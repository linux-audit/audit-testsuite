#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fanotify.h>
#include <unistd.h>

/* Read all available fanotify events from the file descriptor 'fd' */

static void
handle_events(int fd)
{
	const struct fanotify_event_metadata *metadata;
	struct fanotify_event_metadata buf[200];
	ssize_t len;
	char path[PATH_MAX];
	ssize_t path_len;
	char procfd_path[PATH_MAX];
	struct fanotify_response response;

	/* Loop while events can be read from fanotify file descriptor */

	for(;;) {

		/* Read some events */

		len = read(fd, (void *) &buf, sizeof(buf));
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		/* Check if end of available data reached */

		if (len <= 0)
			break;

		/* Point to the first event in the buffer */

		metadata = buf;

		/* Loop over all events in the buffer */

		while (FAN_EVENT_OK(metadata, len)) {

			/* Check that run-time and compile-time structures match */

			if (metadata->vers != FANOTIFY_METADATA_VERSION) {
				fprintf(stderr,
					"Mismatch of fanotify metadata version.\n");
				exit(EXIT_FAILURE);
			}

			/* metadata->fd contains either FAN_NOFD, indicating a
			   queue overflow, or a file descriptor (a nonnegative
			   integer). Here, we simply ignore queue overflow. */

			if (metadata->fd >= 0) {

				/* Handle open permission event */

				if (metadata->mask & FAN_OPEN_PERM) {
					printf("FAN_OPEN_PERM: ");

					/* Allow file to be opened */

					response.fd = metadata->fd;
					response.response = FAN_ALLOW | FAN_AUDIT;
					write(fd, &response,
					      sizeof(struct fanotify_response));
				}

				/* Handle closing of writable file event */

				if (metadata->mask & FAN_CLOSE_WRITE)
					printf("FAN_CLOSE_WRITE: ");

				/* Retrieve and print pathname of the accessed file */

				snprintf(procfd_path, sizeof(procfd_path),
					 "/proc/self/fd/%d", metadata->fd);
				path_len = readlink(procfd_path, path,
						    sizeof(path) - 1);
				if (path_len == -1) {
					perror("readlink");
					exit(EXIT_FAILURE);
				}

				path[path_len] = '\0';
				printf("File %s\n", path);

				/* Close the file descriptor of the event */

				close(metadata->fd);
			}

			/* Advance to next event */

			metadata = FAN_EVENT_NEXT(metadata, len);
		}
	}
}

int
main(int argc, char *argv[])
{
	int fd, poll_num;
	nfds_t nfds;
	struct pollfd fds[2];

	/* Check mount point is supplied */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s MOUNT\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Create the file descriptor for accessing the fanotify API */
	fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK |
			   FAN_ENABLE_AUDIT,
			   O_RDONLY);
	if (fd == -1) {
		perror("fanotify_init");
		exit(EXIT_FAILURE);
	}

	/* Mark the mount for:
	   - permission events before opening files
	   - notification events after closing a write-enabled
	     file descriptor */
	if (fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
			  FAN_OPEN_PERM | FAN_CLOSE_WRITE, -1,
			  argv[1]) == -1) {
		perror("fanotify_mark");
		exit(EXIT_FAILURE);
	}

	/* Prepare for polling */
	nfds = 1;

	/* Fanotify input */
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	/* This is the loop to wait for incoming events */
	printf("Listening for events.\n");

	while (1) {
		poll_num = poll(fds, nfds, -1);
		if (poll_num == -1) {
			if (errno == EINTR)     /* Interrupted by a signal */
				continue;           /* Restart poll() */

			perror("poll");         /* Unexpected error */
			exit(EXIT_FAILURE);
		}

		if (poll_num > 0) {
			if (fds[0].revents & POLLIN) {

				/* Fanotify events are available */
				handle_events(fd);
			}
		}
	}

	printf("Listening for events stopped.\n");
	exit(EXIT_SUCCESS);
}

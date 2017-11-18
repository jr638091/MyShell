e (read(history_fd, &t, 1) != EOF) {
    write(fd_destiny, &t, 1);
    if (t == '\n') {
      write(fd_destiny, itoa(lcount), strlen(itoa(lcount)));
      lcount++;
      write(fd_destiny, ": ", 3);
    }
  }
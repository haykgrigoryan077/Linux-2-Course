#include <iostream>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int
main ()
{
  const char *shm_name = "shared_mem";
  const size_t shm_size = 1024;
  void *shm_ptr;

  int shm_fd = shm_open (shm_name, O_RDONLY, 0);
  if (shm_fd == -1)
    {
      perror ("Failed to open shared memory");
      exit (1);
    }

  shm_ptr = mmap (NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED)
    {
      perror ("Failed to mmap shared memory");
      exit (1);
    }

  while (true)
    {
      char *data = static_cast < char *>(shm_ptr);
      for (int i = 0; i < shm_size; i++)
	{
	  cout << data[i];
	} sleep (5);
    }

  return 0;
}

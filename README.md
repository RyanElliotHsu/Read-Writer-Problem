# Read-Writer-Problem
A starvation-free solution to the classic reader-writer problem using semaphores and shared memory.

Overview:
The purpose of this assignment is to solve the reader-writer problem using shared memory and semaphores. The main structure of the program uses a for loop and fork() calls to create child processes. Within each child process, an exec() call is invoked to call either the “reader” or “writer” program. The processes then execute their respective programs concurrently with other processes. In order to fulfill concurrency, and avoid mishandling of data, we use multiple semaphores to control the flow of the program. To ensure that when a writer process is in its critical section, no other process is accessing the data file, we use a semaphore to lock other processes out, until the writer process releases the lock. Similarly, when readers are accessing the data file, writers are locked out. To prevent starvation, we put processes into a FIFO (first in, first out) queue. Processes are allowed access to the data file in an orderly fashion determined by the queue, this stops processes from “cutting in line”, thereby preventing starvation.

Reader:
In this program, invoked by fork() and exec() calls from the main program, we use the fstream library to access the data file. We use the getline() function to skip through lines before the lower bound, then print out the content in the range of lines given by the main program.

Writer:
Similar to the reader program, fork(), exec(), and the fstream library are used. Here, we open the original CSV file, and create a new CSV file. First, we iterate through the lines before the lower bound, and copy the content into the new CSV file. Now, we move on to the records within the lower and upper bound. We copy the student ID and student name into the new file, and randomly assign processes a value. This value decides whether or not a specific record’s grades will change, if yes, new grades are assigned to each student using the rand() function. Additionally, we calculate the new average GPA of these students to ensure that the data is consistent, then add the new course grades and average GPA to the new file. Finally, we copy the remaining lines after the upper bound into the new file. At this point, creation of the new file is complete, so we remove the original file and rename the new file so that the name of the correct data file is consistent throughout the program.

References:
Some of the program was inspired by ideas and code found in the following websites:
https://stackoverflow.com/questions/48190685/shared-memory-after-exec
https://github.com/aitalshashank2/starve-free-reader-writers-problem

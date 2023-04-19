/*There are several types of Inter-Process Communication (IPC) mechanisms available in Linux for communication between processes:

Pipes: Pipes are a simple and common way to communicate between two processes. A pipe is a unidirectional communication channel between two processes, where one process writes to the pipe and the other process reads from the pipe.

FIFOs: FIFOs, also known as named pipes, are similar to pipes but allow communication between any number of processes. FIFOs are created using the mkfifo system call and behave like regular files.

System V IPC: System V IPC provides three types of interprocess communication mechanisms: message queues, shared memory, and semaphores. These mechanisms use a common set of system calls and data structures.

POSIX IPC: POSIX IPC provides four types of interprocess communication mechanisms: message queues, shared memory, semaphores, and mutexes. These mechanisms conform to the POSIX standard and have a simpler interface than System V IPC.

Signals: Signals are used to notify a process of an event or to interrupt a process. A signal is a software interrupt that is sent to a process to indicate an event has occurred.

Sockets: Sockets provide a way for processes to communicate over a network or between processes on the same machine. They are commonly used for client-server communication.

These IPC mechanisms have different strengths and weaknesses, and the choice of which mechanism to use depends on the specific requirements of the application.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main()
{
    int fd[2];
    pid_t pid;
    char message[] = "Hello, pipe!";

    // Create a pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return 1;
    }

    if (pid == 0) { // Child process
        // Close the unused write end of the pipe
        close(fd[WRITE_END]);

        // Read the message from the pipe
        char buffer[100];
        read(fd[READ_END], buffer, sizeof(buffer));
        printf("Child received message: %s\n", buffer);

        // Close the read end of the pipe
        close(fd[READ_END]);
    } else { // Parent process
        // Close the unused read end of the pipe
        close(fd[READ_END]);

        // Write the message to the pipe
        write(fd[WRITE_END], message, sizeof(message));

        // Close the write end of the pipe
        close(fd[WRITE_END]);
    }

    return 0;
}
/*
FIFO example:
Although pipes are a simple, flexible, and efficient communication mechanism, they
have one main drawback—namely, that there is no way to open an already existing
pipe. This makes it impossible for two arbitrary processes to share the same pipe,
unless the pipe was created by a common ancestor process.
This drawback is substantial for many application programs. Consider, for instance,
a database engine server, which continuously polls client processes wishing to issue
some queries and which sends the results of the database lookups back to them.
Each interaction between the server and a given client might be handled by a pipe.
However, client processes are usually created on demand by a command shell when
a user explicitly queries the database; server and client processes thus cannot easily
share a pipe.
*/
#define FIFO_NAME "/tmp/myfifo"
void use_fifo() {
    pid_t pid1,pid2;
    int fd;
    char buffer[256];
    mkfifo(FIFO_NAME,0666);
    pid1=fork();
    if (pid1==0){
        fd = open(FIFO_NAME, O_WRONLY);
        wrtie(fd, "Hello from process 1 ", sizeof("Hello from process 1 "));
        close(fd);
    } else {
        pid2 =fork();
        if (pid2==0){
            fd =open(FIFO_NAME, O_RDONLY);
            read(fd, buffer, sizeof(buffer));
            printf("process 2 received buffer %s ", buffer);
            close(fd);
        }
        else {
            waitpid(pid1,NULL,0);
            waitpid(pid2,NULL,0);
            // Parent process writes to the FIFO
            fd = open(FIFO_NAME, O_WRONLY);
            write(fd, "Hello from Process 3!", sizeof("Hello from Process 3!"));
            close(fd);
        }
    }
    return 0;
}
/*
Using semaphores 
*/
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

sem_t empty, full, mutex;

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

void *producer(void *arg) {
    int i, item;
    for (i = 0; i < 10; i++) {
        item = rand() % 100;  // Generate a random item
        sem_wait(&empty);     // Wait for an empty slot in the buffer
        sem_wait(&mutex);     // Lock the critical section
        buffer[in] = item;    // Add the item to the buffer
        printf("Producer produced item: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;  // Update the index of the next empty slot
        sem_post(&mutex);     // Unlock the critical section
        sem_post(&full);      // Signal that there is a new item in the buffer
        sleep(1);             // Wait for a second
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int i, item;
    for (i = 0; i < 10; i++) {
        sem_wait(&full);      // Wait for a new item in the buffer
        sem_wait(&mutex);     // Lock the critical section
        item = buffer[out];   // Remove the item from the buffer
        printf("Consumer consumed item: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;  // Update the index of the next item to remove
        sem_post(&mutex);     // Unlock the critical section
        sem_post(&empty);     // Signal that there is an empty slot in the buffer
        sleep(1);             // Wait for a second
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t tid_producer, tid_consumer;
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_create(&tid_producer, NULL, producer, NULL);
    pthread_create(&tid_consumer, NULL, consumer, NULL);
    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer, NULL);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    return 0;
}
/*
Message Queue : Note that message queues are typically used for interprocess communication between unrelated processes, while semaphores are typically used for synchronization between threads in the same process, also for large amounts of data
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFFER_SIZE 5

struct msgbuf {
    long mtype;
    int mtext;
};

int msgqid;

void *producer(void *arg) {
    int i, item;
    struct msgbuf msg;
    msg.mtype = 1;
    for (i = 0; i < 10; i++) {
        item = rand() % 100;  // Generate a random item
        msg.mtext = item;     // Set the message text to the item
        msgsnd(msgqid, &msg, sizeof(int), 0);  // Send the message
        printf("Producer produced item: %d\n", item);
        sleep(1);             // Wait for a second
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int i, item;
    struct msgbuf msg;
    msg.mtype = 1;
    for (i = 0; i < 10; i++) {
        msgrcv(msgqid, &msg, sizeof(int), 1, 0);  // Receive the message
        item = msg.mtext;     // Extract the item from the message
        printf("Consumer consumed item: %d\n", item);
        sleep(1);             // Wait for a second
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t tid_producer, tid_consumer;
    key_t key = ftok("/tmp", 'a');  // Generate a unique key for the message queue
    msgqid = msgget(key, 0666 | IPC_CREAT);  // Create the message queue
    pthread_create(&tid_producer, NULL, producer, NULL);
    pthread_create(&tid_consumer, NULL, consumer, NULL);
    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer, NULL);
    msgctl(msgqid, IPC_RMID, NULL);  // Remove the message queue when done
    return 0;
}

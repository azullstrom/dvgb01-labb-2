#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

struct processes {
    int pid;
    int arrivalTime;
    int burstTime;
    int turnaroundTime;
    int waitingTime;
    int completionTime;
    int remainingTime;
    int completed;
};

struct processes allProcesses[MAX];

int count = 1;

void printAverage() {
    float averageWaitingTime = 0, averageTurnaroundTime = 0;
    for(int i = 0; i < count; i++) {
        averageWaitingTime += allProcesses[i].waitingTime;
        averageTurnaroundTime += allProcesses[i].turnaroundTime;
    }
    averageWaitingTime = averageWaitingTime / count;
    averageTurnaroundTime = averageTurnaroundTime / count;

    printf("\nAverage waiting time: %.2f ms\n", averageWaitingTime);
    printf("Average turnaround time: %.2f ms\n\n", averageTurnaroundTime);
}

void sortProcessesArrivalTime() {
    for(int i = 0; i < count; i++) {
        for(int j = i + 1; j < count; j++) {
            // Sorterar arrival time i ordning om i större än j
            if(allProcesses[i].arrivalTime > allProcesses[j].arrivalTime) {
                // Byter plats på pid
                int temp = allProcesses[i].pid;
                allProcesses[i].pid = allProcesses[j].pid;
                allProcesses[j].pid = temp;

                // Byter plats på arrival time
                temp = allProcesses[i].arrivalTime;
                allProcesses[i].arrivalTime = allProcesses[j].arrivalTime;
                allProcesses[j].arrivalTime = temp;

                // Byter plats på burst time
                temp = allProcesses[i].burstTime;
                allProcesses[i].burstTime = allProcesses[j].burstTime;
                allProcesses[j].burstTime = temp;
            }

            // Om två eller flera processer har samma arrival time
            if(allProcesses[i].arrivalTime == allProcesses[j].arrivalTime) {
                if(allProcesses[i].pid > allProcesses[j].pid) {
                    // Byter plats på pid
                    int temp = allProcesses[i].pid;
                    allProcesses[i].pid = allProcesses[j].pid;
                    allProcesses[j].pid = temp;

                    // Byter plats på arrival time
                    temp = allProcesses[i].arrivalTime;
                    allProcesses[i].arrivalTime = allProcesses[j].arrivalTime;
                    allProcesses[j].arrivalTime = temp;

                    // Byter plats på burst time
                    temp = allProcesses[i].burstTime;
                    allProcesses[i].burstTime = allProcesses[j].burstTime;
                    allProcesses[j].burstTime = temp;
                }
            }
        }
    }
}

// Kör processerna i ordning beroende på arrival time
void fcfsAlgorithm() {
    printf("Scheduling algorithm: FCFS\n\n");

    sortProcessesArrivalTime();

    int gantPos = 0;
    for(int i = 0; i < count; i++) {
        if(i != 0) {
            // Om den nuvarande arrival time är större eller lika med förra processens completion time blir gantPos arrival time
            if(allProcesses[i].arrivalTime >= allProcesses[i-1].completionTime) {
                gantPos = allProcesses[i].arrivalTime; 
                allProcesses[i].completionTime = gantPos + allProcesses[i].burstTime; 
                allProcesses[i].turnaroundTime = allProcesses[i].completionTime - allProcesses[i].arrivalTime; 
                allProcesses[i].waitingTime = allProcesses[i].turnaroundTime - allProcesses[i].burstTime; 
            } else {
                gantPos = allProcesses[i-1].completionTime; 
                allProcesses[i].completionTime = gantPos + allProcesses[i].burstTime; 
                allProcesses[i].turnaroundTime = allProcesses[i].completionTime - allProcesses[i].arrivalTime; 
                allProcesses[i].waitingTime = allProcesses[i].turnaroundTime - allProcesses[i].burstTime; 
            }
        } else { 
            // Om den lägsta arrival time är större än 0 så förskjuts allas arrival time bakåt
            if(allProcesses[i].arrivalTime == 0) {
                gantPos = allProcesses[i].burstTime; 
                allProcesses[i].completionTime = gantPos; 
                allProcesses[i].turnaroundTime = allProcesses[i].completionTime - allProcesses[i].arrivalTime; 
                allProcesses[i].waitingTime = allProcesses[i].turnaroundTime - allProcesses[i].burstTime; 
            } else { 
                int newArrivalTime = allProcesses[i].arrivalTime;
                for(int j = 0; j < count; j++) {
                    allProcesses[j].arrivalTime -= newArrivalTime;
                }
                gantPos = allProcesses[i].burstTime; 
                allProcesses[i].completionTime = gantPos; 
                allProcesses[i].turnaroundTime = allProcesses[i].completionTime - allProcesses[i].arrivalTime; 
                allProcesses[i].waitingTime = allProcesses[i].turnaroundTime - allProcesses[i].burstTime;
            }
        }
    }

    printf("PID\tWaiting Time (ms)\tTurnaround Time (ms)\n");
    for(int i = 0; i < count; i++) {
        printf("%d\t\t\t%d\t\t\t%d\n", allProcesses[i].pid, allProcesses[i].waitingTime, allProcesses[i].turnaroundTime);
    }

    printAverage();
}
/******************************************************************************/
// Kollar alla nästkommande CPU bursts och hittar den kortaste
// Om två delar platsen som kortaste CPU burst så gäller FCFS
// Om en process med kortare CPU burst kommer under tiden en annan process körs så byter SJF till denna process.
void sjfAlgorithm() {
    printf("Scheduling algorithm: SJF\n\n");

    sortProcessesArrivalTime();

    // Om den lägsta arrival time är större än 0 så förskjuts allas arrival time bakåt
    if(allProcesses[0].arrivalTime != 0) {
        int newArrivalTime = allProcesses[0].arrivalTime;
        for(int i = 0; i < count; i++) {
            allProcesses[i].arrivalTime -= newArrivalTime;
        }
    } 

    allProcesses[0].waitingTime = 0;
    allProcesses[0].turnaroundTime = allProcesses[0].burstTime + allProcesses[0].waitingTime;
    allProcesses[0].completionTime = allProcesses[0].burstTime;
    int gantPos = allProcesses[0].completionTime;
    for(int i = 1; i < count; i++) {
        // Om CPUn inväntar nästa process
        if(gantPos < allProcesses[i].arrivalTime) {
            gantPos = allProcesses[i].arrivalTime;
        }

        // Om gantPos är större eller lika med någon av nästkommande processer -> Sortera dessa processers burst time stigande ordning.
        // Dessa processer är i ready state mao och kommer sorteras i ordningen dom ska exekveras enligt SJF.
        for(int j = i + 1; j < count; j++) {
            if(gantPos >= allProcesses[j].arrivalTime) {
                if(allProcesses[i].burstTime > allProcesses[j].burstTime) {
                    // Byter plats på pid
                    int temp = allProcesses[i].pid;
                    allProcesses[i].pid = allProcesses[j].pid;
                    allProcesses[j].pid = temp;

                    // Byter plats på arrival time
                    temp = allProcesses[i].arrivalTime;
                    allProcesses[i].arrivalTime = allProcesses[j].arrivalTime;
                    allProcesses[j].arrivalTime = temp;

                    // Byter plats på burst time
                    temp = allProcesses[i].burstTime;
                    allProcesses[i].burstTime = allProcesses[j].burstTime;
                    allProcesses[j].burstTime = temp;
                }
            }
        }

        // Sortera arrival time om två processer har samma burst time
        for(int k = 0; k < count; k++) {
            for(int j = k + 1; j < count; j++) {
                if(allProcesses[k].burstTime == allProcesses[j].burstTime) {
                    if(allProcesses[k].arrivalTime > allProcesses[j].arrivalTime) {
                        printf("%d %d\n", k, j);
                        // Byter plats på pid
                        int temp = allProcesses[k].pid;
                        allProcesses[k].pid = allProcesses[j].pid;
                        allProcesses[j].pid = temp;

                        // Byter plats på arrival time
                        temp = allProcesses[k].arrivalTime;
                        allProcesses[k].arrivalTime = allProcesses[j].arrivalTime;
                        allProcesses[j].arrivalTime = temp;

                        // Byter plats på burst time
                        temp = allProcesses[k].burstTime;
                        allProcesses[k].burstTime = allProcesses[j].burstTime;
                        allProcesses[j].burstTime = temp;
                    }
                }
            }
        }

        // Om arrival time på processen är större än gantPos så vet vi att waiting time kommer att vara 0 eftersom gantPos 
        // är förra processens completion time
        if(allProcesses[i].arrivalTime > gantPos) {
            allProcesses[i].waitingTime = 0;
            allProcesses[i].turnaroundTime = allProcesses[i].waitingTime + allProcesses[i].burstTime;
            //allProcesses[i].completionTime = allProcesses[i].burstTime;
        } else {
            allProcesses[i].waitingTime = gantPos - allProcesses[i].arrivalTime;
            allProcesses[i].turnaroundTime = allProcesses[i].waitingTime + allProcesses[i].burstTime;
            //allProcesses[i].completionTime = allProcesses[i].burstTime;
        }
        gantPos += allProcesses[i].burstTime;
    }

    printf("PID\tWaiting Time (ms)\tTurnaround Time (ms)\n");
    for(int i = 0; i < count; i++) {
        printf("%d\t\t\t%d\t\t\t%d\n", allProcesses[i].pid, allProcesses[i].waitingTime, allProcesses[i].turnaroundTime);
    }

    printAverage();
}

/******************************************************************************/

int queue[100];
int front = -1, rear = -1;
void enqueue(int i) {
    rear++;
    queue[rear] = i;
    if(front == -1) {
        front = 0;
    }
}

int dequeue() {
    int temp = queue[front];
    if(front == rear) {
        front = rear = -1;
    } else {
        front++;
    }
    return temp;
}

int isInQueue(int i) {
    int k;
    for(k = front; k <= rear; k++) {
        if(queue[k] == i) {
            return 1;
        }
    }
    return 0;
}

void rrAlgorithm(int timeSlice) {
    printf("Scheduling algorithm: RR\n");
    printf("Time quantum: %d ms\n\n", timeSlice);

    sortProcessesArrivalTime();

    // Om den lägsta arrival time är större än 0 så förskjuts allas arrival time bakåt
    if(allProcesses[0].arrivalTime != 0) {
        int newArrivalTime = allProcesses[0].arrivalTime;
        for(int i = 0; i < count; i++) {
            allProcesses[i].arrivalTime -= newArrivalTime;
        }
    } 

    int i, j, burstSum = 0, time = 0, queued = 0;
    // Initialiserar remainingTime och completed och sätter burstSum till alla processers burst time
    for(i = 0; i < count; i++) {
        allProcesses[i].remainingTime = allProcesses[i].burstTime;
        allProcesses[i].completed = 0;
        burstSum += allProcesses[i].burstTime;
    }

    enqueue(0); // Köar första processen

    // Medan time är mindre än burstSum, som en while-loop
    for(time = allProcesses[0].arrivalTime; time < burstSum;) {
        i = dequeue(); // dequeue returnerar indexet först i kön och tar sedan bort det.

        // Om processer har mindre eller lika kvarvarande tid som timeSlice
        if(allProcesses[i].remainingTime <= timeSlice) {
            time += allProcesses[i].remainingTime;
            allProcesses[i].remainingTime = 0;
            allProcesses[i].completed = 1;
            allProcesses[i].waitingTime = time - allProcesses[i].arrivalTime - allProcesses[i].burstTime;
            allProcesses[i].turnaroundTime = time - allProcesses[i].arrivalTime;
            
            // Köar processerna som har kommit under tiden
            for(j = 0; j < count; j++) {
                if(allProcesses[j].arrivalTime <= time && allProcesses[j].completed != 1 && isInQueue(j) != 1) {
                    enqueue(j);
                    queued = 1;
                } 
                if(j == count - 1 && queued != 1 && allProcesses[1].completed != 1 && isInQueue(1) != 1) {
                    queued = 1; 
                    time = allProcesses[1].arrivalTime;
                    burstSum += time - allProcesses[0].burstTime;
                    enqueue(1);
                }
            }
        } else { // Om processer har mer kvarvarande tid än timeSlice
            time += timeSlice;
            allProcesses[i].remainingTime -= timeSlice;
            
            // Köar processerna som har kommit under tiden
            for(j = 0; j < count; j++) {
                if(allProcesses[j].arrivalTime <= time && allProcesses[j].completed != 1 && i != j && isInQueue(j) != 1) {
                    enqueue(j);
                }
            }
            enqueue(i); // Sen köas processen som är preemted
        }
    }

    printf("PID\tWaiting Time (ms)\tTurnaround Time (ms)\n");
    for(int i = 0; i < count; i++) {
        printf("%d\t\t\t%d\t\t\t%d\n", allProcesses[i].pid, allProcesses[i].waitingTime, allProcesses[i].turnaroundTime);
    }

    printAverage();
}

int main(int argc, char* argv[]) {
    char fcfs[] = "FCFS", sjf[] = "SJF", rr[] = "RR";
    int fcfsFalse, sjfFalse, rrFalse;
    char *ret;
    FILE *fp;

    fp = fopen(argv[1], "r");
    // Om filen inte existerar
    if(fp == NULL) {
        printf("Error: File not found\n");
        exit(EXIT_FAILURE);
    }

    // Kollar antalet rader
    char ch;
    do {
        ch = fgetc(fp);
        if(ch == '\n') { 
            count++;
        }
    } while (ch != EOF);
    rewind(fp);

    // Lägger in pid, arrival och burst time separerat med komma (%*c) i processes-structen
    for(int i = 0; i < count; i++) {
        fscanf(fp, "%d%*c %d%*c %d ", &allProcesses[i].pid, &allProcesses[i].arrivalTime, &allProcesses[i].burstTime);
    }
    fclose(fp);

    // Om strängen innehåller .txt eller inte
    ret = strstr(argv[1], ".txt");
    if(!ret) {
        printf("Error: First parameter has to be the file.txt and the second parameter has to be the algorithm name. (FCFS, SJF, RR)\n");
        exit(1);
    } else {
        printf("\nProcess information file: %s\n", argv[1]);
    }
    
    // Om argumenten inte är 3 eller 4 (./sched .txt arg)
    if(argc < 3) {
        printf("Error: Less parameters than allowed\n");
        exit(2);
    } else if(argc > 4) {
        printf("Error: More parameters than allowed\n");
        exit(3);
    }

    // Bestämmer vilken algoritm som ska köras
    fcfsFalse = strcmp(argv[2], fcfs);
    sjfFalse = strcmp(argv[2], sjf);
    rrFalse = strcmp(argv[2], rr);
    
    if(!fcfsFalse) {
        fcfsAlgorithm();
    } else if(!sjfFalse) {
        sjfAlgorithm();
    } else if(!rrFalse) {
        if(argc != 4) {
            printf("Error: You are trying to run RR without time slice. Format: ./sched file.txt RR integer\n");
            exit(4);
        }
        char *p;
        long timeSlice = strtol(argv[3], &p, 10);
        rrAlgorithm(timeSlice);
    } else {
        printf("Error: First parameter has to be the file.txt and the second parameter has to be the algorithm name. (FCFS, SJF, RR)\n");
        exit(1);
    }

    return 0;
}
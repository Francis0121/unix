#include "../smallsh.h"

/**
* 서로 다른 2개의 프로그래을 실행시킨다.
* 먼저 프로그램 한개를 실행시킨뒤 해당 프로그램에서 또다시 fork 하여 pipe를 구현 pipe를 실행시킨다
* .
* @param cline
* 	type char ** : argument list
* @param index
* 	type int * : each pipe line argument start index & index[0] = pipe size
* @param where
* 	type int : process execution position
* @return
* 	type int : isComplete
*/
int pipe_exec(char **cline, int *index, int where){
	int i, j, pfd[2];
	int size, pos;
	pid_t pid;
	char **arg, **arg2;

	if (pipe(pfd) < 0)
		perror("pipe error\n");

	for(i=0; i<index[0]; i++) {

		switch (fork()) {
			case -1 :
				perror("Process[0] fork error\n");
			case 0 :
				if (close(1) == -1)
					perror("Process[0] close error\n");
				if (dup(pfd[1]) != 1)
					perror("Process[0] dup error\n");
				if (close(pfd[0]) == -1 || close(pfd[1]) == -1)
					perror("Process[0] close error");

				// i 가 0 인경우에는 처음 fork 하는 것임으로 parameter의 개수는 index[1] 의 값이다.
				// 그 이후에 값들은 index[2] - index[1]인데 그전의 argument 개수에서 지금 argument의 개수를 뺀것이다.
				size = i == 0 ? index[1] : index[i + 1] - index[i];
				arg = (char **) malloc(sizeof(char *) * size);
				for(j=0; j<size; j++){
					*(arg+j) = (char *) malloc(sizeof(char) * (strlen( *(cline+j) )+2) );
					memcpy( (arg+j), (cline+j), strlen(*(cline+j))+1 );
//					printf("Arg %s\nCline %s\n", *(arg+j), *(cline+j));
				}
				*(arg+j) = (char *) malloc(sizeof(char) * 1);
				*(arg+j) = NULL;

				execvp(*arg, arg);
				perror(*arg);
				exit(0);
		}

		switch(fork()) {
			case -1 :
				perror ("Process[1] fork error\n");
			case 0	:
				if(close(0) == -1)
					perror("Process[1] close error\n");
				if(dup(pfd[0]) != 0)
					perror("Process[1] dup error\n");
				if(close(pfd[0]) == -1 || close(pfd[1]) == -1)
					perror("Process[1] close error\n");

				size = i+1 == index[0] ? strlen(*cline) - index[1] : index[i+2] - index[i+1];
				pos = i+1 == index[0] ? index[i+1] : strlen(*cline);
				arg2 = (char **) malloc(sizeof(char *) * size);
				for(j=0; j<size; j++){
					*(arg2+j) = (char *) malloc(sizeof(char) * (strlen( *(cline+j+pos) )+2) );
					memcpy( (arg2+j), (cline+j+pos), strlen(*(cline+j+pos) )+1 );
				//	printf("Arg2 %s\nCline %s\n", *(arg2+j), *(cline+j+pos));
				}
				*(arg2+j) = (char *) malloc(sizeof(char) * 1);
				*(arg2+j) = NULL;

				execvp(*arg2, arg2);
				perror(*arg2);
				exit(0);
		}
	}



	if(close(pfd[0]) == -1 || close(pfd[1]) == -1)
		perror("close error\n");
	while(wait(NULL) != -1);
	return 0;
}

/**
* PIPE를 실행시키기 위해 독립적인 Process를 만든다.
*
* @param cline
* 	type char ** : argument list
* @param index
* 	type int * : each pipe line argument start index & index[0] = pipe size
* @param where
* 	type int : process execution position
* @return
* 	type int : isComplete
*/
int excapeZombie(char **cline, int *index, int where){
	pid_t pid;

	if ( (pid = fork()) < 0)
		perror("fork error\n");
	else if (pid == 0) {

		if ( (pid = fork()) < 0)
			perror("fork error\n");
		else if (pid > 0) {
			exit(0);
		}

		/*while(getppid() != 1)
			sleep(1);*/
		sleep(2);

		// ~ Escape Zombie process
		// Do something
		/*if(runPipeCommand(cline, index, where) < 0)
			perror("pipe error\n");*/
	}

	if (waitpid(pid, NULL, 0) != pid)
		perror("waitpid error\n");

	return (1);
}
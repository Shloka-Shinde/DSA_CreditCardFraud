#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"credit.h"
#include<string.h>

int main() {
	
	Map *m = initHashMap();
	FILE *fp = fopen("users.csv", "r");
	
	if(fp == NULL) {
		printf("There was some error opening the file \n");
	}
	
	else {
		readUsersData(m, &fp);

		int k = 0;
	
		long int no;
		int i = 0;
		char pass[20], ch, num[16], *endptr;
		
		
		printf(YELLOW "ENter card no. \n");
		
		while((ch = getchar()) != '\n') {
			num[i++] = ch;
		}
		
		num[i] = '\0';
		i = 0;
		
		no = strtol(num, &endptr, 16);
	
		if (endptr == num) {
      		printf("No digits were found.\n");
   		} else if (*endptr != '\0') {
      		printf("Invalid character: %c\n", *endptr);
   		} else {
      		printf("The number is: %ld\n", no);
   		}
	
		printf(YELLOW "ENter password \n");
		
		while((ch = getchar()) != '\n') {
			pass[i++] = ch;
		}
		pass[i] = '\0';
		printf("%s\n", pass);
		
		int result = checkUser(m, no, pass);

		if (result == 1) {
		    
		    printf(CYAN "Login successful.\n");
		    
		    item *endUser = m->array[hashfunction(no)];
		    
		    //char *fileName = (char*)malloc(sizeof(char)*20);
		    //strcpy(fileName, endUser->client.name);
		    
		    //strcat(fileName, ".csv");
		    
		    fp = fopen("new_user.csv", "r");
		    
		    if(fp == NULL) {
		    	printf(RED "There was some error in loading the data \n");
		    }
		    
		    else { 
		    
		    	init_dll(&(endUser->list));
		    	readCsv(&(endUser->list), &fp);
		    	
		    	node *head = copyList(endUser->list);
		    	
		    	endUser->root = sortedToBST(head);
		    	endUser->mean = calculateMean(&(endUser->list));
		    	endUser->stdDev = calculateStandardDeviation(&(endUser->list));
		    	
		    	free(head);
		    	
		    	int option;
		    	while(1) {
		
				printMenu();
				scanf("%d", &option);
				int res = getInput(option, endUser->list, endUser);
				if(res == 0) {
					break;
				}
		    	
		    	}
		    	struct tm t;
		    	t.tm_hour = 12;
		    	t.tm_min = 13;
		    	t.tm_sec = 1;
		    	
		    	TrainModel(endUser, "India", t, 24000, 's');
		   	//detectFraud(endUser);
		   }
		
		} 
		
		else if (result == 0) {
		
		    printf("Incorrect password.\n");
		} 
		
		else if (result == -1) {
		
		    printf("User not found.\n");
		}
				
	}
	
	return 0;
}

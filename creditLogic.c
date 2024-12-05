#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"credit.h"
#include <string.h>
#include<math.h>

char *getLine(FILE **fp) {
	
	char *line = (char*)malloc(sizeof(char)*300);
	strcpy(line, "");
	
	char value;
	int i = 0;
	
	while(fscanf((*fp), "%c", &value) == 1) {
		if(value == '\n') {
			break;
		}
		
		line[i++] = value;
	}	
	
	line[i] = '\0';
	return line;
}

Map *initHashMap() {
	
	Map *hashmap;
	hashmap = (Map*)malloc(sizeof(Map));
	hashmap->array = (item**)malloc(sizeof(item*)*MAPSIZE);
	
	for(int i = 0; i < MAPSIZE; i++) {
		hashmap->array[i] = NULL;
	}
	
	hashmap->size = MAPSIZE;
	hashmap->count = 0;
	
	return hashmap;
} 

/**
 * Function: 
 * Computes a hash value for a given credit card number to map it to an index in the hash table.
 * The function processes the card number in chunks of 4 digits, mixes the chunks using a prime multiplier (37), 
 * and ensures the resulting hash value fits within the hash table size (MAPSIZE).
 * This approach helps distribute card numbers uniformly across the hash table.
 */

int hashfunction(long int card_no) {

    unsigned long long a = card_no;
    unsigned long long hash = 0;

    while (a > 0) {
        hash += (a % 10000);    // Extract last 4 digits
        hash *= 37;             // Mix with a prime multiplier
        a /= 10000;             // Remove processed digits
    }

    // Ensure the hash value fits within the hash table size
    return (unsigned int)(hash % MAPSIZE);   
}


/**
 * Inserts a new user into the hash map using quadratic probing for collision resolution.
 * It calculates the hash index for the user's credit card number and creates a new item 
 * containing the user's details, including name, card number, CVV, expiry date, address, and password.
 * The function attempts to place the item in the hash table, and if collisions occur, it uses 
 * quadratic probing to find the next available slot within the table.
 */

void enter_users(Map *h, user a) {
	
	int idx = hashfunction(a.cardNo);
	
	item *new_item = (item*)malloc(sizeof(item));
	
	strcpy(new_item->client.name, a.name);
	new_item->client.cardNo = a.cardNo;
	new_item->client.cvv = a.cvv;
        new_item->client.expiryDate.day = a.expiryDate.day;
        new_item->client.expiryDate.month = a.expiryDate.month;
        new_item->client.expiryDate.year = a.expiryDate.year;
        strcpy(new_item->client.password, a.password);
        strcpy(new_item->client.address.country, a.address.country);
        strcpy(new_item->client.address.state, a.address.state);
        strcpy(new_item->client.address.city, a.address.city);
        new_item->root = NULL;
        new_item->next = NULL;

	item** arr = h->array;
		
	// using quadratic probing for this : 	
	int i = 0;
	
	while (i < MAPSIZE) {
		unsigned int probeIndex = (idx + i * i) % MAPSIZE;
		if (arr[probeIndex] == NULL) { // Slot is empty
		    arr[probeIndex] = new_item;
		    return;
		}
		i++;
    	}
    	
    	return;		
}

/* Function: Reads user data from a file and populates the hash map with user details. */

void readUsersData(Map *map, FILE **fp) {
	
	char *line;
	
	line = getLine(fp);
	
	while(1) {
	
		line = getLine(fp);
		user client = {0};
		
		if(strcmp(line, "") == 0) {
			break;
		}
		
		char *token = strtok(line, ",");
		strncpy(client.name, token, sizeof(client.name) - 1);
    		client.name[sizeof(client.name) - 1] = '\0';
		
		token = strtok(NULL, ",");
		char *endptr;
		client.cardNo = strtol(token, &endptr, 16);
		if (*endptr != '\0') {
			printf("Error: Invalid card number format\n");
			free(line);
			return;
		}
		
		token = strtok(NULL, ",");
		client.cvv = atoi(token);
		
		token = strtok(NULL, ",");
		if (sscanf(token, "%d/%d/%d", &client.expiryDate.day, &client.expiryDate.month, &client.expiryDate.year) != 3) {
        		printf("Error: Invalid expiry date format\n");
        		free(line);
        		return;
    		}
    		
    		token = strtok(NULL, ",");
		strncpy(client.password, token, sizeof(client.password) - 1);
    		client.password[sizeof(client.password) - 1] = '\0';
		
		token = strtok(NULL, ",");
		strncpy(client.address.country, token, sizeof(client.address.country) - 1);
    		client.address.country[sizeof(client.address.country) - 1] = '\0';
    		
    		token = strtok(NULL, ",");
		strncpy(client.address.state, token, sizeof(client.address.state) - 1);
    		client.address.state[sizeof(client.address.state) - 1] = '\0';
    		
    		token = strtok(NULL, ",");
		strncpy(client.address.city, token, sizeof(client.address.city) - 1);
    		client.address.city[sizeof(client.address.city) - 1] = '\0';
		
		
		enter_users(map, client);
		free(line);
	}	
	
	free(line);
}	

/**
 * This function is responsible for transforming the input password string by applying a transformation to each character.
 * The transformation modifies the ASCII value of each character based on a simple rule:
 *    - If the character's ASCII value is less than 90 (uppercase letters), 15 is added to it.
 *    - If the character's ASCII value is 90 or higher, 16 is subtracted from it.
 * 
 * The function then returns the transformed password as a new string.
 * Use Case:
 *  - This function is used for password encryption for storage or validation.
 */
 
char *checkPass(char *input) {
    int asc, i = 0;
    char *line;

    line = (char*)malloc(sizeof(char) * 20); // Fixed size for simplicity, can be dynamically resized if needed.

    while (input[i] != '\0') {
        asc = (int)input[i];

        if (asc < 90) {
            asc = asc + 15;
        } else {
            asc = asc - 16;
        }
        line[i] = (char)asc;
        i++;
    }
    line[i] = '\0';
    return line; // Remember to free this memory after use in checkUser
}

// * This function checks if a user exists in the hash map and if the provided password matches the stored password. 
int checkUser(Map *map, long int no, char *pass) {
    int idx = hashfunction(no);
    
    if (map->array[idx] == NULL) {
        return -1;  // Return -1 indicating "user not found"
    }

    item *temp = map->array[idx];

    while (temp != NULL) {
    	
        if (temp->client.cardNo == no) {
            // Transform the input password
            char *new = checkPass(pass);

            // Compare the transformed password
            if (strcmp(new, temp->client.password) == 0) {
                free(new);  // Free the allocated memory for transformed password
                return 1; 
                 // Password matched
            } 
            
            else {
                free(new);  // Free the allocated memory for transformed password
                return 0;  // Incorrect password
            }
        }

        temp = temp->next;
    }

    return -1;  // Return -1 indicating "user not found"
}

void init_dll(dll *list) {

	list->head = NULL;
	list->end = NULL;
}

node* createNode(int transaction_id, date payment_date, struct tm payment_time, location payment_place, int zip_code, float amount, char status) {
    
    node* newNode = (node*)malloc(sizeof(node));
    newNode->transaction_id[0] = transaction_id;
    newNode->date_of_payment = payment_date;
    newNode->time_of_payment = payment_time;
    newNode->payment_place = payment_place;
    newNode->zipCode[0] = zip_code;
    newNode->amount = amount;
    newNode->status = status;
    newNode->prev = NULL;
    newNode->next = NULL;
    
    return newNode;
}

void insertEnd(dll* list, node* newNode) {
    if (list->head == NULL) {
        list->head = newNode;
        list->end = newNode;
    } 
    
    else {
        list->end->next = newNode;
        newNode->prev = list->end;
        list->end = newNode;
    }
}

void readCsv(dll *list, FILE **fp) {

	char *line;
	
	// to skip the first line;
	line = getLine(fp);
	
	while(1) {
	
		line = getLine(fp);
		
		if(strcmp(line, "") == 0) {
			break;
		}
		
		char *token = strtok(line, ",");
		int transaction_id = atoi(token);
		
		// Date
		token = strtok(NULL, ",");
		date payment_date;
		sscanf(token, "%d-%d-%d", &payment_date.day, &payment_date.month, &payment_date.year);
		
		// Time
		token = strtok(NULL, ",");
		struct tm payment_time;
		sscanf(token, "%d:%d:%d", &payment_time.tm_hour, &payment_time.tm_min, &payment_time.tm_sec);
		
		// City
		token = strtok(NULL, ",");
		location payment_place;
		strncpy(payment_place.city, token, sizeof(payment_place.city) - 1);
		payment_place.city[sizeof(payment_place.city) - 1] = '\0';
		
		
		 // State
		token = strtok(NULL, ",");
		strncpy(payment_place.state, token, sizeof(payment_place.state) - 1);
		payment_place.state[sizeof(payment_place.state) - 1] = '\0'; // Ensure null-termination
		
		//country 
		token = strtok(NULL, ",");
		strncpy(payment_place.country, token, sizeof(payment_place.country) - 1);
		payment_place.state[sizeof(payment_place.country) - 1] = '\0'; // Ensure null-termination
		
		token = strtok(NULL, ",");
        	int zip_code = atoi(token);
		
		// Amount
		token = strtok(NULL, ",");
		float amount = atof(token);
		
		//status 
		token = strtok(NULL, ",");
        	char status = token[0];
        	
        	node* newNode = createNode(transaction_id, payment_date, payment_time, payment_place, zip_code, amount, status);
        	insertEnd(list, newNode);
	}
}

/* Temporaryfunction only for purpose of testing  : */

void display(dll list) {
	
	node *temp = list.head;
	while(temp != NULL) {
		printf(" %f \n", temp->amount);
		temp = temp->next;
	}	
}

node *copyList(dll list) {
	node *temp = list.head;
	node *p;
	node dummy;
	
	p = &dummy;
	
	while(temp != NULL) {
		node *new = (node*)malloc(sizeof(node));
		new->transaction_id[0] = temp->transaction_id[0];
		new->time_of_payment = temp->time_of_payment;
		new->payment_place = temp->payment_place;
		new->zipCode[0] = temp->zipCode[0];
		new->amount = temp->amount;
		new->status = temp->status;
		
		p->next = new;
		p = p->next;
		
		temp = temp->next;
	} 
	
	p->next = NULL;
	
	return dummy.next;
}	

node* findMiddle(node *head) {
    
    if (head == NULL) return NULL;
    node *slow = head;
    node *fast = head;
    node *prev = NULL;

    while (fast != NULL && fast->next != NULL) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // Disconnect left half
    if (prev != NULL) {
        prev->next = NULL;
    }

    return slow; // Middle node
}

transaction *sortedToBST(node *head) {
	
	if(head == NULL) return NULL;
	
	node *new = findMiddle(head);
    	transaction* root = (transaction*)malloc(sizeof(transaction));
    	root->date_of_payment = new->date_of_payment;
    	root->time_of_payment = new->time_of_payment;
    	root->payment_place = new->payment_place;
    	root->amount = new->amount;
    	root->status = new->status;
    	
    	if(head == new) {
		root->left = NULL;
		root->right = NULL;
        
    	}
    	
    	else {
        
		root->left = sortedToBST(head); // Left half
		root->right = sortedToBST(new->next); // Right half
    	}
    	
    	return root;
}

void traversal(transaction *root) {
	
	if(root != NULL) {
		traversal(root->left);
		printf(" %f \n", root->amount);
		traversal(root->right);	
	}
}

float calculateMean(dll *list) {
    node *temp = list->head;
    float mean = 0.00;
    int count = 0;
    
    // Traverse the linked list and sum the amounts
    while (temp != NULL) {
        mean = mean + temp->amount;  // Accumulate the amount
        count++;  // Increment the count
        temp = temp->next;  // Move to the next node
    }

    // Check for an empty list (avoid division by zero)
    if (count == 0) {
        printf("List is empty, returning 0.0\n");
        return 0.0;
    }

    // Calculate mean
    mean = mean /((float)count);

    return mean;
}

// Function to calculate the standard deviation of transaction amounts
// Formula:
// Standard Deviation (σ) = sqrt( (1/n) * Σ(xi - μ)^2 )
// Where:
// - xi is each individual transaction amount
// - μ is the mean of all transaction amounts
// - n is the total number of transactions
// The function calculates the squared difference between each transaction amount and the mean, 
// sums these squared differences, and returns the square root of the average squared difference.

float calculateStandardDeviation(dll *list) {
    float mean = calculateMean(list);
    node *temp = list->head;
    int count = 0;
    float sum = 0.0;
    while(temp != NULL) {
        
        float diff = temp->amount - mean;
        sum += diff * diff;
        count++;
        temp = temp->next;
        
    }
    return sqrt(sum / (float)count);
}

void printMenu() {

	printf("\n------------------------------------------MENU-------------------------------------------------\n");
	printf(CYAN"1. Recent transactions \n");
	printf(CYAN"2. See transaction history (recent to first) \n");
	printf(CYAN"3. See transaction history (oldest to newwest) \n");
	printf(CYAN"4. Transactions done on particular date \n");
	printf(CYAN"5. Transaction done on particular location \n");
	printf(CYAN"6. Statistics and Insights \n");
	printf(CYAN"7. Search for any potential risks/ fraud alerts\n");
	printf(CYAN"8. Exit Program \n");
}

int getInput(int num, dll list, item *endUser) {
	
	switch(num) {
	
		case 1 : {
			printRecent(list);
			break;
		}
		
		case 2 : {
			RecentToLast(list);
			break;
		}
		
		case 3 : {
			oldTonew(list);
			break;
		}
		
		case 4 : {
			
			printf("\n Enter the date dd mm yyyy\n");
			date target;
			scanf("%d %d %d", &(target.day), &(target.month), &(target.year));
			find_transactions_by_date(endUser->root, target);
			
			break;
		}
		
		case 7 : {
			fraudAlert(list, endUser);
			break;
		}	
		
		case 8 : {
			return 0;
		}
		
		default : {
			printf("Invalid input \n");
		}
	}
	
	return 1;
}

void printRecent(dll list) {
	
	node *temp = list.end;
	int i = 0;
	
	while(i < 10) {
	
		printf("Transaction : \n");
		printf("%d/%d/%d at %s at %d:%d:%d \n", temp->date_of_payment.day, temp->date_of_payment.month, temp->date_of_payment.year, temp->payment_place.city, temp->time_of_payment.tm_hour, temp->time_of_payment.tm_min, temp->time_of_payment.tm_sec );
		
		printf("Amount %f \n", temp->amount);
		
		if(temp->status == 'S' || temp->status == 's') 
			printf(" Status : Successful \n");
			
		else 
			printf(" Status : Failed\n");
			
		printf("----------------------------------------- \n");
		temp = temp->prev;
		i++;
	} 
}

void RecentToLast(dll list) {
	
	node *temp = list.end;
	
	while(temp != NULL) {
		printf("Transaction : \n");
		printf("%d/%d/%d at %s at %d:%d:%d \n", temp->date_of_payment.day, temp->date_of_payment.month, temp->date_of_payment.year, temp->payment_place.city, temp->time_of_payment.tm_hour, temp->time_of_payment.tm_min, temp->time_of_payment.tm_sec );
		
		printf("Amount %f \n", temp->amount);
		
		if(temp->status == 'S' || temp->status == 's') 
			printf(" Status : Successful \n");
			
		else 
			printf(" Status : Failed\n");
			
		
		printf("----------------------------------------- \n");
		temp = temp->prev;
	} 
}

void oldTonew(dll list) {

	node *temp = list.head;
	
	while(temp != NULL) {
	
		printf("Transaction : \n");
		printf("%d/%d/%d at %s at %d:%d:%d \n", temp->date_of_payment.day, temp->date_of_payment.month, temp->date_of_payment.year, temp->payment_place.city, temp->time_of_payment.tm_hour, temp->time_of_payment.tm_min, temp->time_of_payment.tm_sec );
		
		printf("Amount %f \n", temp->amount);
		
		if(temp->status == 'S' || temp->status == 's') 
			printf(" Status : Successful \n");
			
		else 
			printf(" Status : Failed\n");
			
		
		printf("----------------------------------------- \n");
		temp = temp->next;
	}
	
}

int compareDate(date d1, date d2) {
    
    if (d1.year < d2.year) return -1;
    if (d1.year > d2.year) return 1;

    if (d1.month < d2.month) return -1;
    if (d1.month > d2.month) return 1;

    if (d1.day < d2.day) return -1;
    if (d1.day > d2.day) return 1;

    return 0;  // If the dates are equal
}

void find_transactions_by_date(transaction *root, date target_date) {
    
    if (root == NULL)
        return;

    if (compareDate(target_date, root->date_of_payment) == -1) {
        find_transactions_by_date(root->left, target_date);
    }
  
    else if (compareDate(target_date, root->date_of_payment) == 1) {
        find_transactions_by_date(root->right, target_date);
    }
  
    else if(compareDate(target_date, root->date_of_payment) == 0){
        
        printf("Transaction : \n");
	printf("%d/%d/%d at %s at %d:%d:%d \n", root->date_of_payment.day, root->date_of_payment.month, root->date_of_payment.year, root->payment_place.city, root->time_of_payment.tm_hour, root->time_of_payment.tm_min, root->time_of_payment.tm_sec );
        
        printf("Amount %f \n", root->amount);
     	
	if(root->status == 'S' || root->status == 's') 
		printf(" Status : Successful \n");
			
	else 
		printf(" Status : Failed\n");
        
    }
    
    return;
}

/* Here 1 means true;
* 0 means false;
*/

int is_odd_hour(struct tm time) {

    if(time.tm_hour < 6 || time.tm_hour > 22) 
    	return 1;
    /* Example: odd hours are before 6 AM and after 10 PM */
    return 0;
}

// Function to check if there are multiple failed transactions
// It traverses the linked list of transactions and counts consecutive failed transactions (status 'f' or 'F')
// If there are 3 or more consecutive failed transactions, it returns 1 (indicating fraud), otherwise 0.

int multiple_failed_transactions(node *temp) {
	
	int count = 0;
	
	while(temp != NULL && (temp->status == 'f' || temp->status == 'F')) {
		
		count++;
		temp = temp->next;
	}
	
	return (count >= 3 ? 1 : 0);
}

// Function to check if two transactions occurred within a small time frame (5 minutes)

int is_small_time_frame(struct tm last_time, struct tm current_time) {

    double seconds_diff = difftime(mktime(&current_time), mktime(&last_time));
    
    return seconds_diff < 300; // Check if the difference is less than 5 minutes
}

// Function to detect frequent transactions within a short time frame (5 minutes)

int frequent_trans(node *temp) {
	
	node *prev = NULL;
	int count = 0;
	
	while(temp != NULL) {

		if(prev != NULL) {
			if(is_small_time_frame(prev->time_of_payment, temp->time_of_payment) == 1 && compareDate(temp->date_of_payment, prev->date_of_payment) == 0) {
				
				count++;
			}
			
			else {
				break;
			}
		}
		
		prev = temp;
		temp = temp->next;
	}
	
	return count;
}

int is_location_anomaly(location current, location last, location home) {
 
	// Check if countries differ :
	if(strcmp(current.country, last.country) == 0 && strcmp(current.country , home.country) == 0) {
		return 0;
	}	
	
	// check if states differ : 
	else if(strcmp(current.state, last.state) == 0 && strcmp(current.state , home.state) == 0) {
		return 0;
	}
	
	return 1;
}

/* transactions will be judged on the following basis : 
*  Odd hours - when the transactions take place at odd hours.
*  New location - the location of the transaction will be checked with the client's address.
*  Small time window for transaction --- such as large transactions in small time window.
				     --- Multiple failed transactions in small time window.
*  Higher transaction amount than usual. 
*  Other factors include z-score as well.		
*/ 

void fraudAlert(dll list, item *endUser) {
	
	node *tmp = list.head;
	while(tmp != NULL) {
		
		/* A Z-score standardizes the comparison of individual transaction amounts by showing how many standard deviations 
		 * an individual transaction is away from the mean. It gives you a sense of whether a transaction is above or 
		 * below average and by how much in terms of standard deviations.
		*/
		
		float zscore = (tmp->amount - endUser->mean)/(endUser->stdDev);
		int isodd = is_odd_hour(tmp->time_of_payment);
		int multipleFailed = multiple_failed_transactions(tmp);
		int frequent_payments = frequent_trans(tmp);
		int diff_loc = 0;
		
		if(tmp->prev != NULL) {
			diff_loc = is_location_anomaly(tmp->payment_place, tmp->prev->payment_place, endUser->client.address);
		}
		 
		if(fabs(zscore) >= 3 || isodd == 1 || multipleFailed == 1 || frequent_payments >= 3 || diff_loc == 1) {
			
			printf(RED"Transaction id : %d \n", tmp->transaction_id[0]);
			printf(RED"%d-%d-%d at ", tmp->date_of_payment.day, tmp->date_of_payment.month, tmp->date_of_payment.year);
			printf(RED"%d:%d:%d \n", tmp->time_of_payment.tm_hour, tmp->time_of_payment.tm_min, tmp->time_of_payment.tm_sec);
			
			if(fabs(zscore) > 3) {
			
				printf(RED"Outlier detected : \n amount (zscore = %f) : %f \n", zscore, tmp->amount);
			}	
			
			if(isodd == 1) {
			
				printf(RED"Payment of %f at odd hour \n", tmp->amount);
			}
			
			if(multipleFailed == 1) {
				
				printf(RED"multiple failed transactions detected \n");
			}
			
			if(frequent_payments > 3) {
				
				printf(RED"Multiple transactions detected \n");
			}
			
			if(diff_loc == 1) {
				
				printf(RED"Transaction at a new loction detected \n");
			}
			
			printf("\n");	
		}	
		
		tmp = tmp->next;
	}

	return;
}

char timeOfDay(struct tm t) {
	
	char d;
	
    // Check if the time is between 6 AM and 11:59 AM
	if(t.tm_hour >= 6 && t.tm_hour <= 11) {
		d = 'm'; // Morning
	}
	// Check if the time is between 12 PM and 11:59 PM
	else if(t.tm_hour >= 12 && t.tm_hour <= 23) {
		d = 'e'; // Evening
	}
	// For all other cases (midnight to 5:59 AM)
	else {
		d = 'o'; // Other (Early morning or midnight)
	}
	
	return d; 
}

int *flag(item *endUser) {
	
	/*This traverses through the list and identifies and flags the transactions as fraud or non fraud. 
	*/
	node *temp = endUser->list.head;
	int *freq = (int*)malloc(sizeof(int)*2);
	int count = 0;
	int cnt = 0;
	
	while(temp != NULL) {
		
		float zscore = (temp->amount - endUser->mean)/(endUser->stdDev);
		int isodd = is_odd_hour(temp->time_of_payment);
		int multipleFailed = multiple_failed_transactions(temp);
		int frequent_payments = frequent_trans(temp);
		int diff_loc = 0;
		
		if(temp->prev != NULL) {
			diff_loc = is_location_anomaly(temp->payment_place, temp->prev->payment_place, endUser->client.address);
		}
		
		if(diff_loc == 1) {
			temp->fraud = 1;
		}
		
		if(fabs(zscore) >= 3 || multipleFailed >= 3) {
			temp->fraud = 1;
		}
		
		if(fabs(zscore) >= 3 || isodd == 1) {
			temp->fraud = 1;
		}
		
		if(frequent_payments == 1){
			temp->fraud = 1;
		}
		
		if(temp->fraud == 1) cnt++;
		
		temp = temp->next;
		count++;
	}
	
	freq[0] = count;
	freq[1] = cnt;
	
	return freq;
}

void findFreq(item *endUser, countAmt *amt_cat, countLoc *loc_cat, countTime *time_cat, countStatus *st_cat) {
	
	node *temp = endUser->list.head;
	
	while(temp != NULL) {
		
		float z = (temp->amount - endUser->mean)/(endUser->stdDev);
		char c, tim;
		struct tm t = temp->time_of_payment;
		
		if(strcmp(temp->payment_place.country, "India") == 0) {
			c = 'i'; //India
		}
		
		else {
			c = 'n'; //non-India
		}
	
		if(t.tm_hour < 6 || t.tm_hour > 11) {
			tim = 'o'; // Outside morning (6 AM - 11 AM)
		}
		else if(t.tm_hour > 6 && t.tm_hour < 16) {
			tim = 'd'; // Daytime (6 AM - 4 PM)
		}
		
		else {
			tim = 'n'; // Night (for any other cases)
		}
		
		if(temp->fraud == 1) {
			
			// x1
			if(fabs(z) <= 1) {
				amt_cat->z1f += 1;
				amt_cat->total_f += 1;
				amt_cat->total += 1;
			}
			
			else if(fabs(z) <= 2) {
				amt_cat->z2f += 1;
				amt_cat->total_f += 1;
				amt_cat->total += 1;
			}
			
			else {
				amt_cat->z3f += 1;
				amt_cat->total_f += 1;
				amt_cat->total += 1;
			}
			
			//x2;
			if(c == 'i') {
				loc_cat->fin += 1; // Fraudulent transaction in India
				loc_cat->total_f += 1;
				loc_cat->total += 1;
			}
			
			else {
				loc_cat->fout += 1; // Fraudulent transaction outside India
				loc_cat->total_f += 1;
				loc_cat->total += 1;
			}
			
			//x3 
			if(tim == 'o') {
				time_cat->odf += 1;
				time_cat->total_f += 1;
				time_cat->total += 1;
			}
			
			else if(tim == 'd') {
				time_cat->df += 1;
				time_cat->total_f += 1;
				time_cat->total += 1;
			}
			
			else {
				time_cat->nf += 1;
				time_cat->total_f += 1;
				time_cat->total += 1;
			}
			
			//x4;
			if(temp->status == 'f' || temp->status == 'F') {
				st_cat->ff += 1; // Failed fraudulent transaction
				st_cat->total_f += 1;
				st_cat->total += 1;
			}
			
			else {
				st_cat->sf += 1; // Successful fraudulent transaction
				st_cat->total_f += 1;
				st_cat->total += 1;
			}
		}
		
		
		else {
			
			// x1
			if(fabs(z) <= 1) {
				amt_cat->z1 += 1;
				amt_cat->total += 1;
			}
			
			else if(fabs(z) <= 2) {
				amt_cat->z2 += 1;
				amt_cat->total += 1;
			}
			
			else {
				amt_cat->z3 += 1;
				amt_cat->total += 1;
			}
			
			//x2;
			if(c == 'i') {
				loc_cat->in += 1; // Non-fraudulent transaction in India
				loc_cat->total += 1;
			}
			
			else {
				loc_cat->out += 1; // Non-fraudulent transaction outside India
				loc_cat->total += 1;
			}
			
			//x3 
			if(tim == 'o') {
				time_cat->od += 1;
				time_cat->total += 1;
			}
			
			else if(tim == 'd') {
				time_cat->d += 1;
				time_cat->total += 1;
			}
			
			else {
				time_cat->n += 1;
				time_cat->total += 1;
			}
				
			//x4;
			if(temp->status == 'f' || temp->status == 'F') {
				st_cat->f += 1; // Failed non-fraudulent transaction
				st_cat->total += 1;
			}
			
			else {
				st_cat->s += 1; // Successful non-fraudulent transaction
				st_cat->total += 1;
			}
			
		}
	
		temp = temp->next;
	}
}

void TrainModel(item *endUser, char *country, struct tm t, float at, char status) {

	int *counts = flag(endUser);
	float y = (float)counts[1] / counts[0]; // Cast counts[1] to float before division
	float zscore = (at - endUser->mean)/(endUser->stdDev);
	char c;
	char tim;
	
	// x1
	if(fabs(zscore) <= 1) {
		zscore = 1;	
	}
	else if(fabs(zscore) <= 2) {
		zscore = 2;
	}
	else {
		zscore = 3;
	}
	
	// x2
	if(strcmp(country, "India") == 0) {
		c = 'i';
	}
	
	else {
		c = 'n';
	}
	
	// x3
	if(t.tm_hour < 6 || t.tm_hour > 11) {
		tim = 'o';
	}
	else if(t.tm_hour > 6 && t.tm_hour < 16) {
		tim = 'd';
	}
	
	else {
		tim = 'n';
	}
	
	countTime time_cat = {0,0,0,0,0,0,0,0};
	countAmt amt_cat = {0,0,0,0,0,0,0,0};
	countLoc loc_cat = {0,0,0,0,0,0};
	countStatus st_cat = {0,0,0,0,0,0};
	
	findFreq(endUser, &amt_cat, &loc_cat, &time_cat, &st_cat);
	
	float x1, x2, x3, x4;
	float x1_f, x2_f, x3_f, x4_f;
	
	
	// x1;
	if(zscore == 1) {
		x1_f = (float)amt_cat.z1f/amt_cat.total_f;
		
		if(x1_f == 0) {
			x1_f = (float)(amt_cat.z1f + 1)/(amt_cat.total_f + 3);
		}
		
		else if(x1_f == 1) {
			x1_f = (float)(1 - epsilon);
		}	
		
		x1 = (float)(amt_cat.z1f + amt_cat.z1)/amt_cat.total;
		
		if(x1 == 0) {
			x1 = (float)(amt_cat.z1f + amt_cat.z1 + 1)/(amt_cat.total + 3);
		}
		
		else if(x1 == 1) {
			x1 = (float)(1 - epsilon);
		}
	}
	
	else if(zscore == 2) {
		
		x1_f = (float)amt_cat.z2f/amt_cat.total_f;
		
		if(x1_f == 0) {
			x1_f = (float)(amt_cat.z2f + 1)/(amt_cat.total_f + 3);
		}
		
		else if(x1_f == 1) {
			x1_f = (float)(1 - epsilon);
		}
		
		x1 = (float)(amt_cat.z2f + amt_cat.z2)/amt_cat.total;
		
		if(x1 == 0) {
			x1 = (float)(amt_cat.z2f + amt_cat.z2 + 1)/(amt_cat.total + 3);
		}
		
		else if(x1 == 1) {
			x1 = (float)(1 - epsilon);
		}
	}
	
	else {
		x1_f = (float)amt_cat.z3f/amt_cat.total_f;
		
		if(x1_f == 0) {
			x1_f = (float)(amt_cat.z3f + 1)/(amt_cat.total_f + 3);
		}
		
		else if(x1_f == 1) {
			x1_f = (float)(1 - epsilon);
		}
		
		x1 = (float)(amt_cat.z3f + amt_cat.z3)/amt_cat.total;
		
		if(x1 == 0.00) {
			x1 = (float)(amt_cat.z3f + amt_cat.z3 + 1)/(amt_cat.total + 3);
		}
		
		else if(x1 == 1) {
			x1 = (float)(1 - epsilon);
		}
	}
	
	// x2: 
	if(c == 'i') {
		x2_f = (float)loc_cat.fin/loc_cat.total_f;
		
		if(x2_f == 0) {
			x2_f = (float)(loc_cat.fin + 1)/(loc_cat.total_f + 2);
		}
		
		else if(x2_f == 1) {
			x2_f = (float)(1 - epsilon);
		}
		
		x2 = (float)(loc_cat.fin + loc_cat.in)/amt_cat.total;
		
		if(x2 == 0) {
			x2 = (float)(loc_cat.fin + loc_cat.in + 1)/(amt_cat.total + 2);
		}
		
		else if(x2 == 1) {
			x2 = (float)(1 - epsilon);
		}
	}
	
	else {
		x2_f = (float)loc_cat.fout/loc_cat.total_f;
	
		if(x2_f == 0) {
			x2_f = (float)(loc_cat.fout + 1)/(loc_cat.total_f + 2);
		}	
		
		else if(x2_f == 1) {
			x2_f = (float)(1 - epsilon);
		}
		
		x2 = (float)(loc_cat.fout + loc_cat.out)/amt_cat.total;
		
		if(x2 == 0) {
			x2 = (float)(loc_cat.fout + loc_cat.out + 1)/(amt_cat.total + 2);
		}
		
		else if(x2 == 1) {
			x2 = (float)(1 - epsilon);
		}	
	}
	
	// x3 
	if(tim == 'o') {
		x3_f = (float)time_cat.odf/time_cat.total_f;
		
		if(x3_f == 0) {
			x3_f = (float)(time_cat.odf + 1)/(time_cat.total_f + 3);
		}
		
		else if(x3_f == 1) {
			x3_f = (float)(1 - epsilon);
		}
		
		x3 = (float)(time_cat.odf + time_cat.od)/time_cat.total;
		
		if(x3 == 0) {
			x3 = (float)(time_cat.odf + time_cat.od + 1)/(time_cat.total + 3);
		}
		else if(x3 == 1) {
			x3 = (float)(1 - epsilon);
		}	
	}
	
	else if(tim == 'd') {
		x3_f = (float)time_cat.df/time_cat.total_f;
		
		if(x3_f == 0) {
			x3_f = (float)(time_cat.df + 1)/(time_cat.total_f + 3);
		}
		
		else if(x3_f == 1) {
			x3_f = (float)(1 - epsilon);
		}
		
		x3 = (float)(time_cat.df + time_cat.d)/time_cat.total;
		
		if(x3 == 0) {
			x3 = (float)(time_cat.df + time_cat.d + 1)/(time_cat.total + 3);
		}
		else if(x3 == 1) {
			x3 = (float)(1 - epsilon);
		}
	}
	
	else {
		x3_f = (float)time_cat.nf/time_cat.total_f;
		
		if(x3_f == 0) {
			x3_f = (float)(time_cat.nf + 1)/(time_cat.total_f + 3);
		}
		
		else if(x3_f == 1) {
			x3_f = (float)(1 - epsilon);
		}
		
		x3 = (float)(time_cat.nf + time_cat.n)/time_cat.total;
		
		if(x3 == 0) {
			x3 = (float)(time_cat.nf + time_cat.n + 1)/(time_cat.total + 3);
		}
		
		else if(x3 == 1) {
			x3 = (float)(1 - epsilon);
		}
	}
	
	// x4 
	if(status == 's' || status == 'S') {
		x4_f = st_cat.sf/st_cat.total_f;
		
		if(x4_f == 0) {
			x4_f = (float)(st_cat.sf + 1)/(st_cat.total_f + 2);
		}
		
		else if(x4_f == 1) {
			x4_f = (float)(1 - epsilon);
		}
		
		x4 = (float)(st_cat.sf + st_cat.s)/st_cat.total; 
		
		if(x4 == 0) {
			x4 = (float)(st_cat.sf + st_cat.s + 1)/(st_cat.total + 2);
		}
		
		else if(x4 == 1) {
			x4 = (float)(1 - epsilon);
		}	
	}
	
	else {
		x4_f = (float)st_cat.ff/st_cat.total_f;
		
		if(x4_f == 0) {
			x4_f = (float)(st_cat.ff + 1)/(st_cat.total_f + 2);
		}
		
		else if(x4_f == 1) {
			x4_f = (float)(1 - epsilon);
		}	
		
		x4 = (float)(st_cat.ff + st_cat.f)/ st_cat.total; 
		
		if(x4 == 0) {
			x4 = (float)(st_cat.ff + st_cat.f + 1)/(st_cat.total + 2); 
		}
		
		else if(x4 == 1) {
			x4 = (float)(1 - epsilon);
		}
	}
	 
	float p = y *(x4_f * x3_f * x2_f * x1_f)/(x1 * x2 * x3 * x4); 	
	printf(" %f ", p);
	
	return;
}

void detectFraud(item *endUser) {
	
	char status[10];
	char location[30];
	struct tm t;
	float amount;
	
	FILE *fp = fopen("recent.txt", "r");
	char *line;
	line = getLine(&fp);
	
	while(1) {
		
		line = getLine(&fp);
	
		if(strcmp(line, "") == 0) {
			break;
		}
		
		char* token = strtok(line, " "); // Get the first token (amount)
		if (token != NULL) {
			amount = atof(token);
			 // Convert the amount string to float
			 printf("%f \n", amount);
		}
		
		token = strtok(NULL, " "); // Get the second token (location)
		if (token != NULL) {
			strcpy(location, token); // Copy to location
			printf(" %s \n", location);
		}
		
		token = strtok(NULL, " "); // Get the third token (time)
    		if (token != NULL) {
        		sscanf(token, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
        		printf(" %d %d %d \n", t.tm_hour, t.tm_min, t.tm_sec);
    		}
    		
    		token = strtok(NULL, " "); // Get the fourth token (status)
		if (token != NULL) {
			strcpy(status, token); // Copy to status
			printf(" %c \n", status[0]);
		}
		
		TrainModel(endUser, location, t, amount, status[0]);
	}
	
	free(line);
}

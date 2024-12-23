#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"credit.h"
#include <string.h>
#include<math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*The function reads one character at a time until it encounters a newline or EOF. If the line has nn characters,
*  the reading process will take O(n).
*/
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

/*The function initHashMap initializes and returns a new hash map data structure.
* Dynamic Allocation of the Hash Map Structure:
* Time complexity : 
* Allocating the Map structure: O(1).
* Allocating the array of item* pointers: O(MAPSIZE).
* Overall Time Complexity: O(MAPSIZE).
*/
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
 * Time complexity : 
 * If the card number has dd digits, the number of iterations is proportional to ⌈d/4⌉⌈d/4⌉, which simplifies to O(d).
 * Each operation inside the loop (%, +, *, /) is O(1).
 * The final modulo operation is O(1).
 * Overall Time complexity - O(d). 
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
 * Load Factor : Number of elements in the map / size of the map ( in this case is 0.4).
 * With a low load factor, the chance of collisions is significantly reduced. 
 * Most insertions will occur directly at the computed hash index, making insertion operations faster.
 * Time Complexity :
 * Average Case: O(1) for a well-distributed hash function and low load factor.
 * Worst Case: O(MAPSIZE) if the table is nearly full or the hash function causes clustering.
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
		    h->count++;
		    return;
		}
		i++;
    	}
    	
    	return;		
}

/* Function: Reads user data from a file and populates the hash map with user details. 
 * Uses strtok function to tokenize the string based on delimeter here ','.
 * The function reads one line at a time using getLine(fp). If function contains L lines (and c characters in each line): 
 * Line parsing using the following functions : 
 * strtok 
 * strtol
 * atoi 
 * sscanf
 * strncpy 
 * Line parsing: O(L⋅(C+K+M⋅F)).
       Hash map insertion: O(L⋅K).

	The dominant factor depends on CC, KK, and MM, but for practical inputs:
	Total Time Complexity=O(L⋅(C+K+M⋅F))
	Total Time Complexity=O(L⋅(C+K+M⋅F))

	Where:

	    L: Number of lines in the file.
	    C: Average number of characters per line.
	    K: Length of the card number (fixed for a specific application).
	    M: Length of the longest string field (e.g., name or address).
	    F: Number of fields in each line.
 */

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
 * Time complexity : 
 * Memory Allocation : O(1).
 * Loop iterates through each character so for n characters, time complexity - O(n).
 * Overall Time Complexity: O(N), where N is the length of the input string.
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

/* Time complexity: 
 * Best Case:
 * The best-case scenario is when the item is found in the first slot, and no collisions occur, O(1).
 * Worst Case:
 * In the worst case, when all the slots are filled with items, the function might need to probe all the slots in the hash map, O(M).
 * (M is Hashmap size).
*/

item *find(Map *map, long int no) {
	
    int idx = hashfunction(no);
    int i = 0;
    while(i < MAPSIZE) {
    	
    	unsigned int probeIndex = (idx + i * i) % map->size;
    	
    	if (map->array[probeIndex] == NULL) {
            break;  // User not found
        }
	
	item *currentItem = map->array[probeIndex];
	if(currentItem->client.cardNo == no) {
		return currentItem;
	}
	
	i++;
    }

    return NULL; 
}

/* This function checks if a user exists in the hash map and if the provided password matches the stored password.
 * Time complexity :
 * Worst-case time complexity: O(M+L).
 * Best-case time complexity: O(L) (when the user is found in the first probe and no collisions occur).

 * Where:

    M is the size of the hash map (number of slots).
    L is the length of the password.
*/

int checkUser(Map *map, long int no, char *pass) {
    
    int idx = hashfunction(no);
    int i = 0;
    while(i < MAPSIZE) {
    	
    	unsigned int probeIndex = (idx + i * i) % map->size;
    	
    	if (map->array[probeIndex] == NULL) {
            break;  // User not found
        }
	
	item *currentItem = map->array[probeIndex];
	if(currentItem->client.cardNo == no) {
		
		char *new = checkPass(pass);
		// Compare the transformed password
            	if (strcmp(new, currentItem->client.password) == 0) {
                	free(new);  // Free the allocated memory for transformed password
                	return 1; 
                	 // Password matched
            	} 
            	
            	else {
                	free(new);  // Free the allocated memory for transformed password
                	return 0;  // Incorrect password
            	}
	}
	
	i++;
    }

    return -1;  // Return -1 indicating "user not found"
}

void init_dll(dll *list) {
	list->head = NULL;
	list->end = NULL;
}

node* createNode(char *id, date payment_date, struct tm payment_time, location payment_place, int zip_code, float amount, char status) {
    
    node* newNode = (node*)malloc(sizeof(node));
    strcpy(newNode->transaction_id, id);
    newNode->date_of_payment = payment_date;
    newNode->time_of_payment = payment_time;
    newNode->payment_place = payment_place;
    newNode->zipCode = zip_code;
    newNode->amount = amount;
    newNode->status = status;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->fraud = 0;
    
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
		
		if(strcmp(line, "") == 0 || line == NULL) {
			break;
		}
		
		char *token = strtok(line, ",");
		char id[20];
		strncpy(id, token, sizeof(id) - 1);
		id[sizeof(id) - 1] = '\0'; // Ensure null-termination
		
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
		payment_place.country[sizeof(payment_place.country) - 1] = '\0'; // Ensure null-termination
		
		token = strtok(NULL, ",");
        	int zip_code = atoi(token);
		
		// Amount
		token = strtok(NULL, ",");
		float amount = atof(token);
		
		//status 
		token = strtok(NULL, ",");
        	char status = token[0];
        	
        	node* newNode = createNode(id, payment_date, payment_time, payment_place, zip_code, amount, status);
        	insertEnd(list, newNode);
        	
        	free(line);
	}
	
	free(line);
}

/*Purpose :  To create a copy of the linked list, so that this could be used to create a binary search tree.
 * Time Complexity : O(N), where N is the number of elements in the doubly linked list.
*/
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
		new->zipCode = temp->zipCode;
		new->amount = temp->amount;
		new->status = temp->status;
		new->date_of_payment = temp->date_of_payment;
		
		p->next = new;
		p = p->next;
		temp = temp->next;
	} 
	
	p->next = NULL;
	
	return dummy.next;
}	

/* The function findMiddle finds the middle node of a singly linked list, which is typically done using 
 * the tortoise and hare approach (using two pointers: slow and fast).
 * it takes half as many iterations to reach the end of the list as the total number of nodes.
 * The overall Time Complexity is O(N).
*/

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

/* Time Complexity: O(NlogN), where N is the total number of nodes.
*/

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



float calculateMean(dll *list) {
    node *temp = list->head;
    float mean = 0.00;
    int count = 0;
    
    // Traverse the linked list and sum the amounts
    while (temp != NULL) {
    
    	if (temp->status == 'S' || temp->status == 's') {
		mean = mean + temp->amount;  // Accumulate the amount
		count++;  // Increment the count
	}
	
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

/* Function to calculate the standard deviation of transaction amounts
 Formula:
 * Standard Deviation (σ) = sqrt( (1/n) * Σ(xi - μ)^2 )
 * Where:
 * - xi is each individual transaction amount
 * - μ is the mean of all transaction amounts
 * - n is the total number of transactions
 * The function calculates the squared difference between each transaction amount and the mean, 
 * sums these squared differences, and returns the square root of the average squared difference.
*/

float calculateStandardDeviation(dll *list) {
    
    float mean = calculateMean(list);
    node *temp = list->head;
    int count = 0;
    float sum = 0.0;
    while(temp != NULL) {
        if (temp->status == 'S' || temp->status == 's') {
		float diff = temp->amount - mean;
		sum += diff * diff;
		count++;
        }
        temp = temp->next;
        
    }
    return sqrt(sum / (float)count);
}

void printMenu() {

	printf(CYAN"\n------------------------------------------MENU-------------------------------------------------\n");
	printf(CYAN"\n1. Recent transactions \n");
	printf(CYAN"2. See transaction history (recent to first) \n");
	printf(CYAN"3. See transaction history (oldest to newwest) \n");
	printf(CYAN"4. Transactions done on particular date \n");
	printf(CYAN"5. Transaction done on particular location \n");
	printf(CYAN"6. Statistics and Insights \n");
	printf(CYAN"7. Show flagged transactions \n");
	printf(CYAN"8. Search for potential frauds from recent transaction\n");
	printf(CYAN"9. Exit Program \n");
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
			int count = 0;
			find_transactions_by_date(endUser->root, target, &count);
			
			if(count == 0) printf(YELLOW"\n No transactions were found for this date.\n");
			break;
		}
		
		case 5 : {
	
			printf("\n Enter the location as City State Country (separated by spaces) \n");
			location place;
			scanf("%s %s %s", place.city, place.state, place.country);
			find_transactions_byLocation(list, place);
			break;
		}
		
		case 6 : {
			
			printf("\n The mean transaction Amount : %f \n", endUser->mean);
			printf("The Standard Deviation : %f \n", endUser->stdDev);
			
			display_graph(endUser);
			
			break;
		}
		
		case 7 : {
			fraudAlert(list, endUser);
			break;
		}
		
		case 8 : {
			
			detectFraud(endUser);
			break;	
		}	
		
		case 9 : {
			return 0;
		}
		
		default : {
			printf("Invalid input \n");
		}
	}
	
	return 1;
}


/*void drawLineGraph(SDL_Renderer *renderer, dll list) {
    
    if (list.head == NULL) {
        return; // No data to display
    }
    
    // Calculate min and max values of the amount to scale them appropriately
    float minAmount = list.head->amount;
    float maxAmount = list.head->amount;
    node* temp = list.head;
    
    while (temp != NULL) {
        if (temp->amount < minAmount) minAmount = temp->amount;
        if (temp->amount > maxAmount) maxAmount = temp->amount;
        temp = temp->next;
    }
    
    // Scaling factors
    float yScale = (maxAmount - minAmount) > 0 ? HEIGHT / (maxAmount - minAmount) : 1;
    int prevX = 50, prevY = HEIGHT - (list.head->amount - minAmount) * yScale;  // Starting point

    temp = list.head;   
    int count = 1;  // Start from 1, adjusting for proper X spacing

    while (temp != NULL) {
        // Map the amount to a Y coordinate (invert Y-axis for SDL)
        int currX = 50 + count * 50; // Adjust X spacing (increase/decrease if needed)
        int currY = HEIGHT - temp->amount;  // Map the amount to screen space (invert Y)

        // Ensure the current Y is within the height bounds
        if (currY > HEIGHT) currY = HEIGHT;
        if (currY < 0) currY = 0;

        // Draw a line between the previous and current point
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);  // Blue color for the curve
        SDL_RenderDrawLine(renderer, prevX, prevY, currX, currY);

        // Set previous coordinates for the next iteration
        prevX = currX;
        prevY = currY;

        count++;
        temp = temp->next;
    }
}*/
void drawLineGraph(SDL_Renderer *renderer, dll list) {
    
    if (list.head == NULL) {
        return; // No data to display
    }
    
    // Calculate min and max values of the amount to scale them appropriately
    float minAmount = list.head->amount;
    float maxAmount = list.head->amount;
    node* temp = list.head;
    
    while (temp != NULL) {
        if (temp->amount < minAmount) minAmount = temp->amount;
        if (temp->amount > maxAmount) maxAmount = temp->amount;
        temp = temp->next;
    }
    // Scaling factors
    //float yScale = (maxAmount - minAmount) > 0 ? HEIGHT / (maxAmount - minAmount) : 1;
    int prevX = 70, prevY = HEIGHT - 50;  // Starting point

    temp = list.head;   
    int count = 1;  // Start from 1, adjusting for proper X spacing

    while (temp != NULL) {
        // Map the amount to a Y coordinate (invert Y-axis for SDL)
        int currX = 50 + count * 50; // Adjust X spacing (increase/decrease if needed)
        int currY = HEIGHT - temp->amount ;  // Map the amount to screen space (invert Y)

        // Ensure the current Y is within the height bounds
        if (currY > HEIGHT) currY = HEIGHT;
        if (currY < 0) currY = 0;	

        // Draw a line between the previous and current point
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);  // Blue color for the curve
        SDL_RenderDrawLine(renderer, prevX, prevY, currX, currY);

        // Set previous coordinates for the next iteration
        prevX = currX;
        prevY = currY;

        count++;
        temp = temp->next;
    }
}


/*void drawAxis(SDL_Renderer *renderer) {
   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black color
    // Draw X axis
    SDL_RenderDrawLine(renderer, 50, HEIGHT - 50, WIDTH - 50, HEIGHT - 50);
    // Draw Y axis
    SDL_RenderDrawLine(renderer, 50, HEIGHT - 50, 50, 50);
}*/

void drawAxis(SDL_Renderer *renderer,  TTF_Font *font) {
   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black color
    // Draw X axis
    SDL_RenderDrawLine(renderer, 70, HEIGHT - 50, WIDTH - 50, HEIGHT - 50);
    // Draw Y axis
    SDL_RenderDrawLine(renderer, 70, HEIGHT - 50, 70, 50);

	renderText(renderer, font, "DATE", WIDTH - 80, HEIGHT - 40);  // X-axis label
    renderText(renderer, font, "AMOUNT", 10, 50); 
}

/*void display_graph(item *endUser) {

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create the window and renderer
    SDL_Window *window = SDL_CreateWindow("Transaction Line Graph", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Clear the renderer and set the background color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
        SDL_RenderClear(renderer);

        // Draw the axis and the line graph
        drawAxis(renderer);
        drawLineGraph(renderer, endUser->list);  // Draw the line graph

        // Present the rendered content
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}*/

void display_graph(item *endUser) {

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
    // Create the window and renderer
    SDL_Window *window = SDL_CreateWindow("Transaction Line Graph", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    int running = 1;
    SDL_Event event;
	TTF_Font *font = TTF_OpenFont("Square.ttf", 16);
	if (!font) {
    	printf("Failed to load font: %s\n", TTF_GetError());
    	return;
	}
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
    }
    
     // Clear the renderer and set the background color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
        SDL_RenderClear(renderer);

        // Draw the axis and the line graph
        drawAxis(renderer, font);
        drawLineGraph(renderer, endUser->list);  // Draw the line graph

        // Present the rendered content
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	TTF_CloseFont(font);
	TTF_Quit();
}


void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    SDL_Color color = {0, 0, 0, 255};  // Black text color
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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
			
		
		printf("\n----------------------------------------- \n");
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
			
		
		printf("\n----------------------------------------- \n");
		temp = temp->next;
	}
	
}

/* This function compares two dates : given if d1 is date appearing first, it returns -1 
* if d1 appears after d2 it returns 1 
* In case both dates are the same the function returns, 0 
*/

int compareDate(date d1, date d2) {
    
    if (d1.year < d2.year) return -1;
    if (d1.year > d2.year) return 1;

    if (d1.month < d2.month) return -1;
    if (d1.month > d2.month) return 1;

    if (d1.day < d2.day) return -1;
    if (d1.day > d2.day) return 1;

    return 0;  
}

/* The time complexity is determined by the number of recursive calls made and 
 * the depth of the tree. In the worst case, the function might visit every node and perform a constant-time comparison at each node.
 * O(N).
 * Best Case: 
*/
void find_transactions_by_date(transaction *root, date target_date, int *count) {
    
    if (root == NULL)
        return;

    if (compareDate(target_date, root->date_of_payment) == -1) {
        find_transactions_by_date(root->left, target_date, count);
    }
  
    else if (compareDate(target_date, root->date_of_payment) == 1) {
        find_transactions_by_date(root->right, target_date, count);
    }
  
    else if(compareDate(target_date, root->date_of_payment) == 0){
        
        find_transactions_by_date(root->left, target_date, count);
        
        (*count) += 1;
        printf("Transaction : \n");
	printf("%d/%d/%d at %s at %d:%d:%d \n", root->date_of_payment.day, root->date_of_payment.month, root->date_of_payment.year, root->payment_place.city, root->time_of_payment.tm_hour, root->time_of_payment.tm_min, root->time_of_payment.tm_sec );
        
        printf("Amount %f \n", root->amount);
     	
	if(root->status == 'S' || root->status == 's') 
		printf(" Status : Successful \n");
			
	else 
		printf(" Status : Failed\n");
        
        
        find_transactions_by_date(root->right, target_date, count);
        
    }
    
    return;
}

void find_transactions_byLocation(dll list, location place) {
	
	node *temp = list.head;
	int count = 0;

	while(temp != NULL) {
		
		if(strcmp(temp->payment_place.country, place.country) == 0  && strcmp(temp->payment_place.state, place.state) == 0 && strcmp(temp->payment_place.city, place.city) == 0)  {
		
			count++;
			printf("Transaction : \n");
			printf("%d/%d/%d at %s at %d:%d:%d \n", temp->date_of_payment.day, temp->date_of_payment.month, temp->date_of_payment.year, temp->payment_place.city, temp->time_of_payment.tm_hour, temp->time_of_payment.tm_min, temp->time_of_payment.tm_sec );
			
			printf("Amount %f \n", temp->amount);
		     	
			if(temp->status == 'S' || temp->status == 's') 
				printf(" Status : Successful \n");
					
			else 
				printf(" Status : Failed\n");
		}
		
		
		temp = temp->next;
	}
	
	if(count == 0) {
		printf(YELLOW"\nNo transactions were found for this location\n");
	}
}

/* Here 1 means true;
* 0 means false;
*/

int is_odd_hour(struct tm time) {

    if(time.tm_hour < 6 || time.tm_hour > 22) 
    	return 1;
    
    /* Example: odd hours are before 6:00:00 and after 22:00:00  */
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

	last_time.tm_isdst = -1;  
	current_time.tm_isdst = -1;

	    
	last_time.tm_mday = 1;   // Set a default day (valid day in tm structure)
	last_time.tm_mon = 0;     // Set to January (valid month)
	last_time.tm_year = 100;  // Set a default year (valid)

	current_time.tm_mday = 1;   // Set a default day (valid day in tm structure)
	current_time.tm_mon = 0;     // Set to January (valid month)
	current_time.tm_year = 100;  // Set a default year (valid)


	time_t last_time_t = mktime(&last_time);
	time_t current_time_t = mktime(&current_time);


	if (last_time_t == (time_t) -1 || current_time_t == (time_t) -1) {
		fprintf(stderr, "Error: mktime failed\n");
		return 0;  // Return some default value if mktime fails
	}

	double seconds_diff = difftime(current_time_t, last_time_t);
	    
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
* Time Complexity : 
* Assuming that the helper functions are efficient and the typical number of consecutive failed or 
* frequent transactions is small, the overall time complexity of the fraudAlert function is: O(N).	
*/ 

void fraudAlert(dll list, item *endUser) {
	
	node *tmp = list.head;
	int count = 0;
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
			
			count++;
			printf(RED"Transaction id : %s \n", tmp->transaction_id);
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
	
	if(count == 0) {
		printf("\nBased on the analysis of your transaction data history, no fraudulent activity has been detected for the recent ones. All transactions appear normal and secure.\n");
		
	}
	
	return;
}

/*This traverses through the list and identifies and flags the transactions as fraud or non fraud. 
* Uses the same conditions to flag the transactions as used in the function fraudAlert.
* Time Complexity : O(N).
*/

int *flag(item *endUser) {
	
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

/* Time Complexity : O(N).
*/
void findFreq(item *endUser, countAmt *amt_cat, countLoc *loc_cat, countTime *time_cat, countStatus *st_cat) {
	
	node *temp = endUser->list.head;
	
	while(temp != NULL) {
		
		float z = (temp->amount - endUser->mean)/(endUser->stdDev);
		char c, tim;
		struct tm t = temp->time_of_payment;
		
		if(strcmp(temp->payment_place.country, "India") == 0) {
			c = 'i'; // india 
		}
		
		else {
			c = 'n'; // not india 
		}
	
		if(t.tm_hour < 6 || t.tm_hour > 22) {
			tim = 'o'; // odd hours
		}
		else if(t.tm_hour > 6 && t.tm_hour < 16) {
			tim = 'm'; //morning or day 
		}
		
		else {
			tim = 'e'; // evening 
		}
		
		if(temp->fraud == 1) {
			
			// x1
			if(fabs(z) <= 1.5) {
				amt_cat->z1f += 1;
				amt_cat->total_f += 1;
				amt_cat->total += 1;
				
			}
			
			else if(fabs(z) <= 3) {
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
				// Fraudulent transaction in India
				loc_cat->fin += 1;
				loc_cat->total_f += 1;
				loc_cat->total += 1;
				
			}
			
			else {
				// Fraudulent transaction outside India
				loc_cat->fout += 1;
				loc_cat->total_f += 1;
				loc_cat->total += 1;
				
			}
			
			//x3 
			if(tim == 'o') {
				time_cat->odf += 1;
				time_cat->total_f += 1;
				time_cat->total += 1;
				
			}
			
			else if(tim == 'm') {
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
				
				// Failed fraudulent transaction
				st_cat->ff += 1;
				st_cat->total_f += 1;
				st_cat->total += 1;
			}
			
			else {
				// Successful fraudulent transaction
				st_cat->sf += 1;
				st_cat->total_f += 1;
				st_cat->total += 1;
				
			}
		}
		
		
		else {
			
			// x1
			if(fabs(z) <= 1.5) {
				amt_cat->z1 += 1;
				amt_cat->total += 1;
			}
			
			else if(fabs(z) <= 3) {
				amt_cat->z2 += 1;
				amt_cat->total += 1;
			}
			
			else {
				amt_cat->z3 += 1;
				amt_cat->total += 1;
			}
			
			//x2;
			if(c == 'i') {
				// Non-fraudulent transaction in India
				loc_cat->in += 1; 
				loc_cat->total += 1;
			}
			
			else {
				// Non-fraudulent transaction outside India
				loc_cat->out += 1;
				loc_cat->total += 1;
			}
			
			//x3 
			if(tim == 'o') {
				time_cat->od += 1;
				time_cat->total += 1;
			}
			
			else if(tim == 'm') {
				time_cat->d += 1;
				time_cat->total += 1;
			}
			
			else {
				time_cat->n += 1;
				time_cat->total += 1;
			}
				
			//x4;
			if(temp->status == 'f' || temp->status == 'F') {
				// Failed non-fraudulent transaction
				st_cat->f += 1;
				st_cat->total += 1;
			}
			
			else {
				// Successful non-fraudulent transaction
				st_cat->s += 1;
				st_cat->total += 1;
			}
			
		}
	
		temp = temp->next;
	}
	
}

/*
    Function: TrainModel
    Purpose: Implements a Naive Bayes classifier to predict whether a credit card transaction is fraudulent or non-fraudulent based on multiple features.
    
    Parameters:
        - item *endUser: Pointer to user-specific transaction statistics (mean, standard deviation).
        - char *country: The country where the transaction occurred.
        - struct tm t: The timestamp of the transaction, used to categorize time.
        - float at: The amount of the transaction.
        - char status: The transaction status ('s' for successful, others for failed).
        - countTime time_cat: Struct holding counts for time categories (morning, evening, odd-hours).
        - countAmt amt_cat: Struct holding counts for Z-score categories (low, moderate, high).
        - countLoc loc_cat: Struct holding counts for location categories (domestic(India), international).
        - countStatus st_cat: Struct holding counts for status categories (success, failure).
        - int *counts: Array where counts[0] is the total number of transactions and counts[1] is the total number of fraudulent transactions.
    
    Returns:
        - Prints whether the transaction is "fraud" or "non fraud" and the probabilities of each.
    */

	/*The Naive Bayes classifier is based on Bayes’ Theorem:
	P(A∣B)=  (P(B∣A)⋅P(A))/ P(B)

	A: The class (Fraud or Non-Fraud).
	B: The observed features (Amount, Location, Time, Status). 
	
         P(Fraud|Features) ∝ P(Fraud) * P(Feature_1|Fraud) * P(Feature_2|Fraud) * ...
         Here, Features include Z-score (transaction amount), location, time of transaction, and transaction status.
*/	

void TrainModel(item *endUser, char *country, struct tm t, float at, char status, countTime time_cat, countAmt amt_cat, countLoc loc_cat, countStatus st_cat, int *counts) {

	// Step 1: Calculate the Z-score to determine how unusual the transaction amount is.
	
	float zscore = (at - endUser->mean)/(endUser->stdDev);
	char c;
	char tim;
	
	// Compute the probability of non-fraudulent (pnf) and fraudulent (pf) transactions:
    	// `y` = P(Fraud), `x` = P(Non-Fraud).
    	
    	
    	// y = P(Fraud) = Total fraudulent transactions / Total transactions
	float y = (float)counts[1]/counts[0]; 
	
	//probability of non fraud transaction // x = P(Non-Fraud) = 1 - P(Fraud)
	float x = 1 - y; 
	int total_nf = counts[0] - counts[1];
	
	// x1
	// Step 2: Categorize the Z-score into three levels based on thresholds (low, moderate, high deviation).

	if(fabs(zscore) <= 1.5) {
		zscore = 1;	
	}
	else if(fabs(zscore) <= 3) {
		zscore = 2;
	}
	else {
		zscore = 3;
	}
	
	// x2
	// Step 3: Categorize the location as domestic or international.
	if(strcmp(country, "India") == 0) {
		c = 'i';
	}
	else {
		c = 'n';
	}
	
	//x3
	
	if(t.tm_hour < 6 || t.tm_hour > 22) {
		tim = 'o';
	}
	else if(t.tm_hour > 6 && t.tm_hour < 16) {
		tim = 'm';
	}
	else {
		tim = 'e';
	}
	
	// Initialize conditional probabilities for each feature
	float x1, x2, x3, x4;
	float x1_f, x2_f, x3_f, x4_f;	
	float pnf, pf;
	
	//Laplace smoothing is applied to avoid zero probabilities.
	/* Laplace smoothing prevents zero probabilities by adding 1 to feature category counts. 
 	This ensures no category is excluded,
 	avoiding a zero posterior probability that would make classification impossible. */
	
	// Step 5: Calculate conditional probabilities for Z-score (amount deviation).
	
	if(zscore == 1) {
		x1_f = (float)(amt_cat.z1f + 1)/(counts[1] + 3);
		x1 = (float)(amt_cat.z1 + 1)/(total_nf + 3);
	}
	
	else if(zscore == 2) {
		x1_f = (float)(amt_cat.z2f + 1)/(counts[1] + 3);
		x1 = (float)(amt_cat.z2 + 1)/(total_nf + 3);
	}
	
	else {
		x1_f = (float)(amt_cat.z3f + 1)/(counts[1] + 3);
		x1 = (float)(amt_cat.z3 + 1)/(total_nf + 3);
	}
	
	//x2: 
	// Step 6: Calculate conditional probabilities for location.
	
	if(c == 'i') {
		x2_f = (float)(loc_cat.fin + 1)/(counts[1] + 2);
		x2 = (float)(loc_cat.in + 1)/(total_nf + 2);
	}
	
	else {
	
		x2_f = (float)(loc_cat.fout + 1)/(counts[1] + 2);
		x2 = (float)(loc_cat.out + 1)/(total_nf + 2);
	}
	
	//x3 
	// Step 7: Calculate conditional probabilities for time.
	
	if(tim == 'o') {
		
		x3_f = (float)(time_cat.odf + 1)/(counts[1] + 3);
		x3 = (float)(time_cat.od + 1)/(total_nf + 3);	
	}
	
	else if(tim == 'm') {
		
		x3_f = (float)(time_cat.df + 1)/(counts[1] + 3);
		x3 = (float)(time_cat.d + 1)/(total_nf + 3);
	}
	
	else {
		x3_f = (float)(time_cat.nf + 1)/(counts[1] + 3);
		x3 = (float)(time_cat.n + 1)/(total_nf + 3);
	}
	
	//x4 
	// Step 8: Calculate conditional probabilities for transaction status.
	
	if(status == 's' || status == 'S') {
		x4_f = (float)(st_cat.sf + 1)/(counts[1] + 2);
		x4 = (float)(st_cat.s + 1)/(total_nf + 2);
	}
	
	else {
		x4_f = (float)(st_cat.ff + 1)/(counts[1] + 2);
		x4 = (float)(st_cat.f + 1)/(total_nf + 2); 
	}
	
	// Step 9: Calculate posterior probabilities for fraud (pf) and non-fraud (pnf).
	
	pnf = x * x1 * x2 * x3 * x4;
	pf = y * x1_f * x2_f * x3_f * x4_f;
	
	
	// Normalize the probabilities (to ensure their sum equals 1).
	float p = pnf + pf;
	pnf = pnf / p;
	pf = pf / p;
	
	// Step 10: Classification decision based on probabilities.
	/*Decision Rule: The transaction is classified as fraudulent if 
	P(Fraud∣Features)>P(Non−Fraud∣Features).
	*/
	
	printf(CYAN"\nFraud Status: ");
	
	if (pnf > pf) {
		printf(GREEN"Non-Fraudulent\n");
		printf("Probability (Non-Fraud): %.2f%% \nProbability (Fraud): %.2f%% ", pnf*100, pf*100);
		
	} else {
		printf(RED"Fraudulent\n");
		printf("Probability (Non-Fraud): %.2f%% \nProbability (Fraud): %.2f%%", pnf*100, pf*100);
	}
	
	return;
}

void detectFraud(item *endUser) {
	
	char status[10];
	char location[30];
	struct tm t;
	float amount;
	
	char *fileName = (char*)malloc(sizeof(char)*40);
	
	strcpy(fileName, endUser->client.name);
	strcat(fileName, "Recent.txt");
		    
	
	FILE *fp = fopen(fileName, "r");
	char *line;
	line = getLine(&fp);
	
	countTime time_cat = {0,0,0,0,0,0,0,0};
	countAmt amt_cat = {0,0,0,0,0,0,0,0};
	countLoc loc_cat = {0,0,0,0,0,0};
	countStatus st_cat = {0,0,0,0,0,0};
	
	int *counts = flag(endUser);
	
	findFreq(endUser, &amt_cat, &loc_cat, &time_cat, &st_cat);
	
	if(counts[1] == 0) printf(CYAN"\nAny previously flagged transactions doesnt exist, hence could not handle the probability!\n");
	
	while(1) {
		
		line = getLine(&fp);
	
		if(strcmp(line, "") == 0) {
			break;
		}
		
		char* token = strtok(line, " "); // Get the first token (amount)
		if (token != NULL) {
			amount = atof(token);
			 // Convert the amount string to float
		}
		
		token = strtok(NULL, " "); // Get the second token (location)
		if (token != NULL) {
			strcpy(location, token); // Copy to location
		}
		
		token = strtok(NULL, " "); // Get the third token (time)
    		if (token != NULL) {
        		sscanf(token, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
    		}
    		
    		token = strtok(NULL, " "); // Get the fourth token (status)
		if (token != NULL) {
			strcpy(status, token); // Copy to status
		}
		
		printf(CYAN"\n-----------------------\n");
		printf(CYAN"\n For Transaction amount : %f at time %d:%d:%d\n", amount, t.tm_hour, t.tm_min, t.tm_sec);
		
		
		if(counts[1] != 0) {
			TrainModel(endUser, location, t, amount, status[0], time_cat, amt_cat, loc_cat, st_cat, counts);
		}
		
		else {
			float zscore = (amount - endUser->mean)/(endUser->stdDev);
			int isodd = is_odd_hour(t);
				
			if(fabs(zscore) > 3 || isodd == 1) {
				printf(RED"\nPotential Fraud Alert \n");
				
				if(fabs(zscore) > 3) {
					printf(RED"Outlier detected : \n amount (zscore = %f) : %f \n", zscore, amount);
				}
				
				if(isodd == 1) {
					printf(RED"Transaction at odd hour, %d:%d:%d\n", t.tm_hour, t.tm_min, t.tm_sec);
				}
			}
			
			else {
				printf(CYAN"\n No risk detected\n");
			}	
		}
		free(line);
	}
	
	free(line);
}

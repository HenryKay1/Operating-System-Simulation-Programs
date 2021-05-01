
#include <stdio.h> 
#include <stdlib.h> 
typedef struct node { 
    int event_type;
    int job_id; 
    int priority; 
  
    struct node* next; 
  
} Node; 




Node* initialize1(int d,int j, int p) 
{ 
  
    Node* initialNode = (Node*)malloc(sizeof(Node)); 
    initialNode-> event_type = d; 
      initialNode->job_id =j;
    initialNode->priority = p;
  
    initialNode->next = NULL; 
  
    return initialNode; 
} 
int getEvent_id(Node** Current) 

{ 
    return (*Current)->event_type; 
} 
int getjob_id(Node** Current) 

{ 
    return (*Current)->job_id; 
} 
int getTime(Node** Current){
  return (*Current)->priority;
}

  
void push(Node** Current, int d,int j, int p) 
{ 
    Node* start = (*Current); 
  
    // Create new Node 

      Node* temp = initialize1(d,j,p);
  
    // Special Case: The Current of list has lesser 
    // priority than new node. So insert new 
    // node before Current node and change Current node. 
    if ((*Current)->priority > p) { 
  
        // Insert New Node before Current 
        temp->next = *Current; 
        (*Current) = temp; 
    } 
    else { 
  
        // Traverse the list and find a 
        // position to insert new node 
        while (start->next != NULL && 
               start->next->priority < p) { 
            start = start->next; 
        } 
  
        // Either at the ends of the list 
        // or at required position 
        temp->next = start->next; 
        start->next = temp; 
    } 
} 
void pop(Node** Current) 
{ 
    Node* temp = *Current; 
    (*Current) = (*Current)->next; 
    free(temp); 
} 
int isEmpty(Node** Current) 
{ 
    return (*Current) == NULL; 
} 
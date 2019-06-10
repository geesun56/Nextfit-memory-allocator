
#include "ku_malloc.h"

	

hnode_t *free_list = NULL;				//Free List header
	
hnode_t *next_pointer = NULL;		//pointer for next-fit




	
void* splitting(void * target_address, size_t newsize){
	hnode_t * target_node= (hnode_t*) target_address;
	hnode_t * search_node= free_list;
	void* allocate_address=NULL;
	
	while(1){
				if(search_node->next==target_node)	break;
				if(search_node->next!=NULL)	search_node=search_node->next;
				}
		
		
	if(search_node->next->size==newsize){	//delete node from free list
		fprintf(stderr,"\n***PERFECT FIT***\n");
		allocate_address=(void*)search_node->next+sizeof(hnode_t);
		search_node->next=search_node->next->next;
		
	}
	else{		//split free space node
		hnode_t * temp_node=search_node->next;
		//printf("current_size test:0x%x\n",temp_node->size);
		allocate_address=(void*)search_node->next+sizeof(hheader_t);
		//printf("allocate_address test: %p\n",allocate_address);
		
		//printf("requested size:0x%x\n",newsize);
		search_node->next=allocate_address+newsize;
		//printf("search_node->next test: %p\n",search_node->next);
		//printf("size test 0x%x 0x%x 0x%x\n",temp_node->size, newsize,sizeof(hnode_t));
		search_node->next->size=temp_node->size-newsize-sizeof(hnode_t);
		search_node->next->next=temp_node->next;
		
		//printf("splitted node test size: 0x%x\n", search_node->next->size);
		//printf("splitted node test size: %p\n", search_node->next);
		
		fprintf(stderr,"\n***AFTER SPLITTING***\n");
	}
	
	print_freelist();
	
	return allocate_address;
	

}
	
	
void* next_fit(size_t size){	//FIND TARGET ADDRESS  
	void * target_address;
	int searching=0;
	hnode_t * search_node = NULL;	
	
	if(next_pointer==NULL){		//-> If there is no history of allocation, set next_pointer as the first node.
		next_pointer=free_list;
		search_node = next_pointer;
	}
	else{
		hnode_t * find_node = free_list;
		
		while(1){
			if((void*)find_node>(void*)next_pointer){
				search_node=find_node;
				break;
			}
			else
				find_node = find_node->next;
			
		}
		
		
		
	}
	
	while(1){	//0: start, 1: not found after next-pointer, 2: found target!
		
		
		//printf("*2\n");
		if(searching==0){			//First time searching
			//printf("*3\n");
			while(1){														
				//printf("*3-1\n");
				
				 if(search_node->next==NULL){
					 //printf("*3-2\n");
						if(search_node->size>=size){
							
							target_address=search_node;
							searching=2;									
							break;
						}
						else{ 
							
							searching=1;
							search_node=free_list;
							break;
						}
				 }
				 else{
					 //printf("*3-3\n");
					if(search_node->size>=size){
						target_address=search_node;
						searching=2;
						break;
					}
					
					search_node=search_node->next;
					}
				}
		}
		
		
		if(searching==1){
			//printf("*4\n");
			while(1){
			  if(search_node->next==NULL){
				 //printf("*4-1\n");
					if(search_node->size>=size){
						target_address=search_node;
						searching=2;
						break;
					}
					else{ 
						searching=3;
						target_address=NULL;
						break;
					}
			 }
			 else{
				 //printf("*4-2\n");
				if(search_node->size>=size){
					target_address=search_node;
					searching=2;
					break;
				}
				
				search_node=search_node->next;
				}
				
			}
		}
		
		
		if(searching==2){	
			//printf("*5\n");
			break;
		}
		
		
		if(searching==3){
			//printf("*6\n");
			fprintf(stderr,"NEXT-FIT Algorithm ERROR!\n"); 
			break;
		}
	}
	
	if(target_address!=NULL) next_pointer=target_address;
	else fprintf(stderr,"\n@@@TARGET ADDRESS ERROR\n");
	
	return target_address;
}





void *malloc(size_t size){	// my allocator
	void *my_pointer;
	void *current_top;
	current_top = sbrk(0);
	//init_freelist();
	
	if(size<0){	// error!
		fprintf(stderr,"allocation error!!!\n");
		my_pointer=NULL;
	}
	else if(size==0){
		my_pointer= (void*)sbrk(0);
		fprintf(stderr,"Current Heap memory TOP: %p\n", my_pointer);
	}
	else{
		int signal = check_freelist(size); //FL_check(size);*** 
		
		fprintf(stderr,"<MEM ALLOCATION>\n");
		if(signal==1){
			fprintf(stderr," case 2: Freespace Allocation\n");
			void *target_address=next_fit(size);
			fprintf(stderr,"target address after next fit: %p\n",target_address);
			my_pointer=splitting(target_address,size);
			hheader_t * newheader = (hheader_t *)target_address;
			newheader->size= size;
			newheader->magic= (unsigned long)my_pointer^KU_MAGIC;
			
			}
		else{
			fprintf(stderr," case 1: Heap Increment\n");
			//can't allocate now -> increase and allocate
			sbrk(size+sizeof(hheader_t));
			my_pointer= current_top+sizeof(hheader_t)+1;
			hheader_t * newheader = (hheader_t *)(current_top+1);
			newheader->size= size;
			newheader->magic= (unsigned long)my_pointer^KU_MAGIC;
			//printf("size test: %x\n", newheader->size);
			//printf("magic test: %x\n", newheader->magic);
		}
		
		fprintf(stderr," allocated address: %p\n", my_pointer);
		
	}
	
	return my_pointer;
	}
	
	


void free(void *ptr){
	hheader_t * check = ptr-sizeof(hheader_t);
	//printf("my_free address: %p\n",check);
	//printf("my_free magic: %x\n",check->magic);
	unsigned long check_magic = (unsigned long)ptr^KU_MAGIC;
	
	if(check->magic == check_magic){
		fprintf(stderr,"-->free space creation\n");
		add_freelist(check->size, (hnode_t*)check);
		
	}
	else{		//Do not Free
		fprintf(stderr,"invalid address!!! \n");
	}
	
	
	
}



void add_freelist(size_t newsize, hnode_t* address){	// free the space and add to freelist
	hnode_t *search_node = free_list;
	hnode_t *newnode = address;
	newnode->size= newsize;
	newnode->next= NULL;
	if(free_list==NULL){	//add first
		free_list=address;
	}
	else{
			if(free_list>newnode){		// add first when only 1 left
				free_list = newnode;
				newnode->next= search_node;
			}
			else{		
				while(free_list<newnode){
					if(search_node->next!=NULL){
						
						if(search_node->next>newnode){
							newnode->next=search_node->next;
							search_node->next=newnode;
							break;	
						}
						else{
							search_node=search_node->next;
						}
					}
					else{
						search_node->next=newnode;
						break;
					}
				}
			}
		}			//add last
		
		print_freelist();
		coalescing();
}

void coalescing(){

	int sig_coal = 0;
	hnode_t * temp;
	while(sig_coal<2){	//start: 0 changed: 1 end: 2
		hnode_t *search_node = free_list;
		sig_coal = 0;
		while(search_node->next!=NULL){
			//printf("2\n");
			//printf("search_node: %p\n", search_node);
			//rintf("search_node->next: %p\n", (unsigned long)search_node->next);
			//printf("coales condition check1: %x \n",(unsigned long)search_node->next-(unsigned long)search_node);
			//printf("coales condition check2: %p \n",search_node->next);
			if((unsigned long)search_node + search_node->size + sizeof(hnode_t) == search_node->next){	//coalescing condition
				temp=search_node->next;
				search_node->size=search_node->size+temp->size+sizeof(hnode_t);
				search_node->next=temp->next;
				sig_coal=1;
				fprintf(stderr,"***COALESCING OCCURED***\n");
				print_freelist();
				break;
			}
			else
				//printf("Coalescing not occured\n");
				search_node=search_node->next;
		}
		
		if(search_node->next==NULL && sig_coal==0)		//no-changed
			sig_coal=2;
	} 
	
}

int check_freelist(size_t size){
	int signal=0;
	//printf("sss\n");
	hnode_t *search_node=free_list;
	if(free_list!=NULL){
		while(1){
			//printf("sss\n");
			if(search_node->size>=size)	signal=1;
			//printf("sss\n");
			if(search_node->next!=NULL) search_node=search_node->next;
			else break;
			
			}
		}
	
	return signal;
}



void print_freelist(){
	int node_number = 0;
	hnode_t *search_node=free_list;
	if(free_list==NULL){	//add first
		fprintf(stderr,"====There is nothing in freelist!!!====\n");
	}
	else{
		fprintf(stderr,"< FREE LIST >\n");
		while(1){
			
			fprintf(stderr," ==free list node %d\n", node_number++);
			fprintf(stderr," # myaddress: %p\n",search_node);
			fprintf(stderr," # size: 0x%x\n",search_node->size);
			fprintf(stderr," # next: %p\n",search_node->next);
			fprintf(stderr," # consecutive address: %p\n",(void*)search_node+search_node->size+sizeof(hnode_t));
			if(search_node->next!=NULL)
				search_node=search_node->next;
			else
				break;
		}
		
	}
	fprintf(stderr,"\n");
}





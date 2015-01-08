
#include "all.h"

// EXAMPLE OF A LINKEDLIST
/*
int main(void)
{
  struct my_list*  mt = NULL;
 
  mt = list_new();
  list_add_element(mt, 1);
  list_add_element(mt, 2);
  list_add_element(mt, 3);
  list_add_element(mt, 4); 
   
  list_print(mt);
 
  list_remove_element(mt);
  list_print(mt);
 
  list_free(mt);    always remember to free() the malloc()ed memory
  free(mt);         free() if list is kept separate from free()ing the structure, I think its a good design 
  mt = NULL;       after free() always set that pointer to NULL, C will run havon on you if you try to use a dangling pointer 
 
  list_print(mt);
 
  return 0;
}
*/
 
 
/* Will always return the pointer to my_list */
struct signal_structure* list_add_element(struct my_list* s, struct signal_structure* sig)
{
  struct list_node* p = malloc(sizeof(struct list_node));
  p->signal = malloc(sizeof(struct signal_structure));
 
  if( NULL == p )
    {
      fprintf(stderr, "IN %s, %s: malloc() failed\n", __FILE__, "list_add");
    }
  memcpy(p->signal, sig, sizeof(struct signal_structure));
  p->next = NULL;
 
 
  if( NULL == s )
    {
      printf("Queue not initialized\n");
      free(p);
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      /* printf("Empty list, adding p->num: %d\n\n", p->num);  */
      s->head = s->tail = p;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      fprintf(stderr, "There is something seriously wrong with your assignment of head/tail to the list\n");
      free(p);
    }
  else
    {
      /* printf("List not empty, adding element to tail\n"); */
      s->tail->next = p;
      s->tail = p;
    }
 
  return p->signal;
}
 
 
/* This is a queue and it is FIFO, so we will always remove the first element */
struct my_list* list_remove_element( struct my_list* s )
{
  struct list_node* h = NULL;
  struct list_node* p = NULL;
 
  if( NULL == s )
    {
      printf("List is empty\n");
      return s;
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      printf("Well, List is empty\n");
      return s;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      printf("There is something seriously wrong with your list\n");
      printf("One of the head/tail is empty while other is not \n");
      return s;
    }
 
  h = s->head;
  p = h->next;
  free(h->signal);
  h->signal = NULL;// Added so that signal_struct is free'd before we free the list_node
  free(h);
  h = NULL;
  s->head = p;
  if( NULL == s->head )  s->tail = s->head;   /* The element tail was pointing to is free(), so we need an update */
 
  return s;
}
   
 
/* ---------------------- small helper fucntions ---------------------------------- */
struct my_list* list_free( struct my_list* s )
{
  while( s->head )
    {
      list_remove_element(s);
    }
 
  return s;
}
 
struct my_list* list_new(void)
{
  struct my_list* p = malloc( 1 * sizeof(*p)); // Should work, may switch to malloc(sizeof(struct my_list)) later
 
  if( NULL == p )
    {
      fprintf(stderr, "LINE: %d, malloc() failed\n", __LINE__);
    }
 
  p->head = p->tail = NULL;
   
  return p;
}
 
 
void list_print( const struct my_list* ps )
{
  struct list_node* p = NULL;
 
  if( ps )
    {
      for( p = ps->head; p; p = p->next )
    {
      list_print_element(p);
    }
    }
 
  printf("------------------\n");
}
 
 
void list_print_element(const struct list_node* p )
{
	// Removed because we don't ever need to print the list elements. Change as needed
	if( p )
    {
      //printf("Num = %d\n", p->num);
    }
	else
    {
      //printf("Can not print NULL struct \n");
    }
}

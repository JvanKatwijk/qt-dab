
#include	"control-queue.h"
#include	<stdio.h>

	controlQueue::controlQueue	() {
}

	controlQueue::~controlQueue	() {
}

int	controlQueue::getHead		() {
int	head	= theQueue. front ();
	theQueue. pop ();
	return head;
}

int	controlQueue::size		() {
	return theQueue. size ();
}

bool	controlQueue::isEmpty		() {
	return theQueue.size () <= 0;
}

void	controlQueue::add		(int key) {
	theQueue. push (key);
}

void	controlQueue::add		(int key, int val) {
	theQueue. push (key);
	theQueue. push (val);
}

void	controlQueue::add		(int key, int v1, int v2) {
	theQueue. push (key);
	theQueue. push (v1);
	theQueue. push (v2);
}


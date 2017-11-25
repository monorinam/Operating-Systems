package hashMap;


public class HashLinkedList<K,V>{
	/*
	 * Fields
	 */
	private HashNode<K,V> head;

	private Integer size;

	/*
	 * Constructor
	 */

	HashLinkedList(){
		head = null;
		size = 0;
	}


	/*
	 *Add (Hash)node at the front of the linked list
	 */

	public void add(K key, V value){
		// ADD CODE BELOW HERE
		//Adapted from Singly LinkedList handout code
		// Adds node to the head of the list, and changes
		// the old node if list is not empty
		HashNode<K,V> newNode = new HashNode<K,V>(key,value);
		size++;
		if (head==null) {
			head = newNode;
			
		}
		else
		{
			newNode.next = head;
			head = newNode;
		}
		// ADD CODE ABOVE HERE
	}

	/*
	 * Get Hash(node) by key
	 * returns the node with key
	 */

	public HashNode<K,V> getListNode(K key){
		// ADD CODE BELOW HERE
		//Get index and then add using the index
		int index = getIndexOf(key);
		if(index < 0 || index >= size)
		{
			return null;
		}
		//HashNode<K,V> tempNode = getNode(index);
		return getNode(index);
		// ADD CODE ABOVE HERE
	}


	/*
	 * Remove the head node of the list
	 * Note: Used by remove method and next method of hash table Iterator
	 */

	public HashNode<K,V> removeFirst(){
		// ADD CODE BELOW HERE
		return remove(0);
		// ADD CODE ABOVE HERE
		//return null; //CODE STUB.. REMOVE THIS LINE
	}

	/*
	 * Remove Node by key from linked list 
	 */

	public HashNode<K,V> remove(K key){
		// ADD CODE BELOW HERE
		int index = getIndexOf(key);
		if(index< 0)
			return null;
		else{
			return remove(index);
		}

		// ADD CODE ABOVE HERE
		//return null; // removing failed
	}



	/*
	 * Delete the whole linked list
	 */
	public void clear(){
		head = null;
		size = 0;
	}
	/*
	 * Check if the list is empty
	 */

	boolean isEmpty(){
		return size == 0? true:false;
	}

	int size(){
		return this.size;
	}

	//ADD YOUR HELPER  METHODS BELOW THIS
	// This is modified (minimally) from SLinkedList.java
	HashNode<K,V> remove(int i){ 
		if ((i < 0) || (i >= size)){
			  throw new IndexOutOfBoundsException();
		}
		else{
			
			//  first deal with special case that size == 1, i == 0
			HashNode<K,V> cur = head;
			if ((size == 1) && (i == 0)){  	//  only one node in list
				size--;
				
				head = null;  
				//tail = null;
				return cur; 
			}
			
			//  Now we can assume that size > 1.   
			//  We first deal with case that i == 0
			
			
			size--;
			if (i == 0){
				head = head.next;
				return cur;
			}
			else{   //  we can assume that i > 0;
				cur = getNode(i-1);
				HashNode<K,V> nodeToRemove = cur.next;
				cur.next = nodeToRemove.next;
				nodeToRemove.next = null;   //  be safe
				return nodeToRemove;
			}
		}
	}
	// This method gets the index of the node with key
	private int getIndexOf(K key){
		HashNode<K,V>  cur = head;
		int i = 0;     
		
		if (cur == null) //empty list
			return -1;
		
		while ((cur.getKey() != key) && (cur.next != null)){
			cur = cur.next; //find key
			i++;
		}
		
		if (cur.getKey() == key)
			return i;
		else 
			return -1;
	}
	// this method replaces the value for a key at a given index with a different one
	// Used to overwrite the value at a key node
	void replace(K key, V value)
	{
		int index = getIndexOf(key);
		if(index == -1)
		{
			add(key,value);
		}
		else
		{
			//iterate over indexes until current
			int i = 0;
			HashNode<K,V> cur = head;
			while(i < index){
				cur = cur.next;
				i++;
			}
			if(cur.getKey() == key)
				cur.setValue(value); //set value
		}
	} 
	// This is a package level helper method to find the element
	// with the given position.
	HashNode<K,V> getNode(int i)
	{
		if(i < 0 || i >=size)
		{
			throw new IndexOutOfBoundsException();
		}
		else{		
			if (i == 0){  	//  only one node in list
				return head; 
			}
			else{
				// Find node with index
				int index = 0;	
				HashNode<K,V> cur = head;
				while (index < i){   
					cur = cur.next;
					index++;
				}
				return cur; 
			}
		}
	}
	
	//ADD YOUR HELPER METHODS ABOVE THIS


}

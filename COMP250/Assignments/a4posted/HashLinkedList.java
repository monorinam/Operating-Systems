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
			return null
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
	public HashNode<K,V> remove(int i){ 
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
			
			//SNode<E> cur = head;
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
	public int getIndexOf(K key){
		HashNode<K,V>  cur = head;
		int i = 0;     
		
		if (cur == null)
			return -1;
		
		while ((cur.getKey() != key) && (cur.next != null)){
			cur = cur.next;
			i++;
		}
		
		if (cur.getKey() == key)
			return i;
		else 
			return -1;
	}
	// this method replaces the value for a key at a given index with a different one
	public void replace(K key, V value)
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
				cur.setValue(value);
		}
	} 
	// This is a private helper method to find the element
	// with the given key.
	private HashNode<K,V> getNode(int i)
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
	// private HashNode<K,V> getNode(K key)
	// {
	// 	HashNode<K,V> curr = head;
	// 	//Find the node with the given key
	// 	for(int i = 0; i < size; i ++)
	// 	{
	// 		if(curr.key == key)
	// 		{
	// 			return curr; //found the right node
	// 		}
	// 		else
	// 		{
	// 			curr = curr.next;
	// 		}
	// 	}
	// 	//If key is not found, then return null
	// 	return null;
	// }


	//ADD YOUR HELPER METHODS ABOVE THIS


}

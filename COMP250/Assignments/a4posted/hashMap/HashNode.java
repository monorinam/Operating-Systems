package hashMap;


public  class HashNode<K,V> implements Cloneable{
	private V value;
	private K key;
	public HashNode<K,V> next;

	/*
	 * Constructor.
	 */

	HashNode(K key, V value){
		this.value = value;
		this.key = key;
		this.next = null;
	}

	//ADD HELPER METHODS BELOW HERE

	// Hint: Implement a  'Public' clone() method
	// Sets the value of the node 
	void setValue(V value)
	{
		this.value = value;
	}
	// Clone method
	@Override
	public HashNode<K,V> clone()
	{
		// This will set the key and value of the clone to the same as this.k/v
		HashNode<K,V> copy = new HashNode<K,V>(this.key, this.value);
		// this will set the next value to the same as this.next
		copy.next = this.next;
		return copy;

	}
	//ADD HELPER METHODS ABOVE HERE

	/*
	 * Returns this hash entry's value. 
	 * Assume entry is not null.
	 */

	public V getValue(){
		return this.value;
	}
	/*
	 * Returns this hash nodes's key.  
	 * Assume entry is not null.
	 * @return This hash nodes's key
	 */

	public K getKey(){
		return this.key;
	}
	/*
	 * Return Next node
	 */
	HashNode<K,V> getNext(){
		return this.next;
	}

	@Override
	public String toString() {
		/*
		 * Implemented method. You do not need to modify.
		 */
		return key.toString()+" : " + value.toString();
		//return  value.toString();
	}

}

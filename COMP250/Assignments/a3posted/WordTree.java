package assignments2017.a3posted;

//COMP 250 - Introduction to Computer Science - Fall 2017
//Assignment #3 - Question 1

import java.util.*;

/*
 *  WordTree class.  Each node is associated with a prefix of some word 
 *  stored in the tree.   (Any string is a prefix of itself.)
 */

public class WordTree
{
	WordTreeNode root;

	// Empty tree has just a root node.  All the children are null.

	public WordTree() 
	{
		root = new WordTreeNode();
	}


	/*
	 * Insert word into the tree.  First, find the longest 
	 * prefix of word that is already in the tree (use getPrefixNode() below). 
	 * Then, add TreeNode(s) such that the word is inserted 
	 * according to the specification in PDF.
	 */
	public void insert(String word)
	{
		//  ADD YOUR CODE BELOW HERE
		// Check if the word exists in the tree
		// Use the string version of the prefix to get prefix length
		String prefix_string = this.getPrefix(word);
		WordTreeNode prefix_node = this.getPrefixNode(word);
		if(prefix_string.length() < word.length())
		{
			//the words are not equal, and nodes need to be added
			for(int i = prefix_string.length(); i < word.length(); i++)
			{
				// Create a childnode and step to the created node
				prefix_node = prefix_node.createChild(word.charAt(i));
			}

		}
		
		//  ADD YOUR ABOVE HERE
	}

	// insert each word in a given list 

	public void loadWords(ArrayList<String> words)
	{
		for (int i = 0; i < words.size(); i++)
		{
			insert(words.get(i));
		}
		return;
	}

	/*
	 * Given an input word, return the TreeNode corresponding the longest prefix that is found. 
	 * If no prefix is found, return the root. 
	 * In the example in the PDF, running getPrefixNode("any") should return the
	 * dashed node under "n", since "an" is the longest prefix of "any" in the tree. 
	 */
	WordTreeNode getPrefixNode(String word)
	{
		//   ADD YOUR CODE BELOW HERE
		// start at the root, then traverse the tree to find the prefix
		WordTreeNode prefix_node = root;
		for(int i = 0; i < word.length(); i ++)
		{
			// if children at word.charAt(i) exists, then found child
			if(prefix_node.getChild(word.charAt(i)) != null)
			{
				//child found, move to child
				prefix_node = prefix_node.getChild(word.charAt(i));
			}
			else
			{
				return prefix_node;	
			}

			// move down the tree
		}
		// this is the end of a word
		this.setEndOfWord(true);

		   //return null;  //  REPLACE THIS STUB
		
		//   ADD YOUR CODE ABOVE HERE

	}

	/*
	 * Similar to getPrefixNode() but now return the prefix as a String, rather than as a TreeNode.
	 */

	public String getPrefix(String word)
	{
		return getPrefixNode(word).toString();
	}


	/*
	 *  Return true if word is contained in the tree (i.e. it was added by insert), false otherwise.
	 *  Hint:  any string is a prefix of itself, so you can use getPrefixNode().
	 */
	public boolean contains(String word)
	{  
		//   ADD YOUR CODE BELOW HERE
		WordTreeNode prefix_node = this.getPrefixNode(word);
		// Check if the prefix exists at all
		if(prefix_node == null)
			return false;
		// compare the two strings
		/*
		Checking for
			- Length of word is same as length of prefix
			- the last word is the end of a word
		*/
		if(this.getPrefix(word).length() == word.length() && prefix_node.isEndOfWord())
			return true;
		return false;   
		
		//   ADD YOUR CODE ABOVE HERE
	}

	/*
	 *  Return a list of all words in the tree that have the given prefix. 
	 *  For example,  getListPrefixMatches("") should return all words in the tree.
	 */
	public ArrayList<String> getListPrefixMatches( String prefix )
	{
		//  ADD YOUR CODE BELOW 

		return null;   //  REMOVE THIS STUB
		
		//  ADD YOUR CODE ABOVE HERE
	}


	/*
	 *  Below is the inner class defining a node in a Tree (prefix) tree.  
	 *  A node contains an array of children: one for each possible character.  
	 *  The children themselves are nodes.
	 *  The i-th slot in the array contains a reference to a child node which corresponds 
	 *  to character  (char) i, namely the character with  ASCII (and UNICODE) code value i. 
	 *  Similarly the index of character c is obtained by "casting":   (int) c.
	 *  So children[97] = children[ (int) 'a']  would reference a child node corresponding to 'a' 
	 *  since (char)97 == 'a'   and  (int)'a' == 97.
	 * 
	 *  To learn more:
	 * -For all unicode charactors, see http://unicode.org/charts
	 *  in particular, the ascii characters are listed at http://unicode.org/charts/PDF/U0000.pdf
	 * -For ascii table, see http://www.asciitable.com/
	 * -For basic idea of converting (casting) from one type to another, see 
	 *  any intro to Java book (index "primitive type conversions"), or google
	 *  that phrase.   We will cover casting of reference types when get to the
	 *  Object Oriented Design part of this course.
	 */

	public class WordTreeNode
	{
		/*  
		 *   Highest allowable character index is NUMCHILDREN-1
		 *   (assuming one-byte ASCII i.e. "extended ASCII")
		 *   
		 *   NUMCHILDREN is constant (static and final)
		 *   To access it, write "TreeNode.NUMCHILDREN"
		 *   
		 *   For simplicity,  we have given each WordTree node 256 children. 
		 *   Note that if our words only consisted of characters from {a,...,z,A,...,Z} then
		 *   we would only need 52 children.   The WordTree can represent more general words
		 *   e.g.  it could also represent many special characters often used in passwords.
		 */

		public static final int NUMCHILDREN = 256;

		WordTreeNode     parent;
		WordTreeNode[]   children;
		int              depth;            // 0 for root, 1 for root's children, 2 for their children, etc..
		
		char             charInParent;    // Character associated with the tree edge from this node's parent 
		                                  // to this node.
		// See comment above for relationship between an index in 0 to 255 and a char value.
		
		boolean endOfWord;   // Set to true if prefix associated with this node is also a word.

		
		// Constructor for new, empty node with NUMCHILDREN children.  
		//  All the children are automatically initialized to null. 

		public WordTreeNode()
		{
			children = new WordTreeNode[NUMCHILDREN];
			
			//   These assignments below are unnecessary since they are just the default values.
			
			endOfWord = false;
			depth = 0; 
			charInParent = (char)0; 
		}


		/*
		 *  Add a child to current node.  The child is associated with the character specified by
		 *  the method parameter.  Make sure you set as many fields in the child node as you can.
		 *  
		 *  To implement this method, see the comment above the inner class TreeNode declaration.  
		 *  
		 */
		
		public WordTreeNode createChild(char  c) 
		{	   
			WordTreeNode child       = new WordTreeNode();

			// ADD YOUR CODE BELOW HERE
			children[(int) c] = child; //add child to children array of this node
			child.parent = this; // the child's parent is this node
			child.charInParent = c;
			child.depth = depth + 1;

			// ADD YOUR CODE ABOVE HERE

			return child;
		}

		// Get the child node associated with a given character, i.e. that character is "on" 
		// the edge from this node to the child.  The child could be null.  

		public WordTreeNode getChild(char c) 
		{
			return children[ c ];
		}

		// Test whether the path from the root to this node is a word in the tree.  
		// Return true if it is, false if it is prefix but not a word.

		public boolean isEndOfWord() 
		{
			return endOfWord;
		}

		// Set to true for the node associated with the last character of an input word

		public void setEndOfWord(boolean endOfWord)
		{
			this.endOfWord = endOfWord;
		}

		/*  
		 *  Return the prefix (as a String) associated with this node.  This prefix
		 *  is defined by descending from the root to this node.  However, you will
		 *  find it is easier to implement by ascending from the node to the root,
		 *  composing the prefix string from its last character to its first.  
		 *
		 *  This overrides the default toString() method.
		 */

		public String toString()
		{
			// ADD YOUR CODE BELOW HERE
			// For each child, the charInParent gives the edge
			if(this.parent == null)
				return null;
			else
				return this.parent.toString() + this.charInParent;

			//return null;   //  REMOVE THIS CODE STUB
			
			// ADD YOUR CODE ABOVE HERE
		}
	}


}
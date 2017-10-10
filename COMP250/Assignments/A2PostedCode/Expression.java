package assignments2017.A2PostedCode;

/*

 */

import java.util.Stack;
import java.util.ArrayList;

public class Expression  {
	private ArrayList<String> tokenList;

	//  Constructor    
	/**
	 * The constructor takes in an expression as a string
	 * and tokenizes it (breaks it up into meaningful units)
	 * These tokens are then stored in an array list 'tokenList'.
	 */

	Expression(String expressionString) throws IllegalArgumentException{
		tokenList = new ArrayList<String>();
		StringBuilder token = new StringBuilder();

		//ADD YOUR CODE BELOW HERE
		//..
		//..
		// Loop to tokenize the string
		for(int i= 0; i<expressionString.length(),i++)
		{
			//Check if string is whitespace, if so, ignore
			if(expressionString.charAt(i) == ' ')
				continue;
			
			//Check if there are invalid characters in the string TODO
			if(!(Character.isDigit(expressionString.charAt(i)) || expressionString.charAt(i) == '(' || expressionString.charAt(i) == ')' || expressionString.charAt(i) =='+' || expressionString.charAt(i) =='-' || expressionString.charAt(i) == '*' || expressionString.charAt(i) == '/' || expressionString.charAt(i) =='[' || expressionString.charAt(i) ==']'))
			{
				throw new IllegalArgumentException();
			}
			token.append(expressionString.charAt(i));
			//If the element is a number, then need to store the full number.
			// not just an element of the number
			if(Character.isDigit(expressionString.charAt(i)))
			{
				//Get the full number string
				while(i<expressionString.length() && Character.isDigit(expressionString.charAt(i+1)))
				{
					token.append(expressionString.charAt(++i));
				}
			}
			// Tokenize ++,--
			if(expressionString.charAt(i) == '+')
			{
				if(expressionString.charAt(i+1) == '+')
				{
					token.append(expressionString.charAt(++i));
				}
			}
			if(expressionString.charAt(i) =='-')
			{
				if(expressionString.charAt(i+1) == '+')
				{
					token.append(expressionString.charAt(++i));
				}
			}
			tokenList.add(token.toString());
			token.delete(0,token.length());

		}
		//ADD YOUR CODE ABOVE HERE
	}

	/**
	 * This method evaluates the expression and returns the value of the expression
	 * Evaluation is done using 2 stack ADTs, operatorStack to store operators
	 * and valueStack to store values and intermediate results.
	 * - You must fill in code to evaluate an expression using 2 stacks
	 */
	public Integer eval(){
		Stack<String> operatorStack = new Stack<String>();
		Stack<Integer> valueStack = new Stack<Integer>();
		
		//ADD YOUR CODE BELOW HERE
		//..
		//..
		// First check if token is bracket, integer or operator and push to stack accordingly
		// If a closing bracket is encountered, evaluate the expression, pop the necessary operators and values
		// and push the evaluated value to the stack
		// So, for (2 + 3 + (++3))
		// Push (, 2, +, 3, +, ++, 3 into either of two stacks, then when ) is seen,
		// evaluate ++3, pop from stack, store 4 on stack. For next ), evaluate 2+3+4 
		// and pop from stack, push 9 to the value stack and return 9 by poping the only element left
		for(int i = 0;i < this.tokenList.size();i++)
		{
			if(this.tokenList.get(i).equals("(") || this.tokenList.get(i).equals("["))
				continue; //do nothing since opening brackets are not stored on stack
			else if (this.tokenList.get(i).isInteger())
			{
				//Push to valueStack
				valueStack.push(this.tokenList.get(i));
			}
			else if(this.tokenList.get(i).equals("+") || this.tokenList.get(i).equals("-") || this.tokenList.get(i).equals("*") || this.tokenList.get(i).equals("/") || this.tokenList.get(i).equals("++") || this.tokenList.get(i).equals("--"))
			{
				// operators pushed to operatorStack
				operatorStack.push(this.tokenList.get(i));
			}
			
			else if (this.tokenList.get(i).equals(')') || this.tokenList.get(i).equals(']'))
			{
				// Evaluate the last operation
				// Pop element from operatorStack for evaluation
				//Using switch case since the element called be popped only once
				// And therefore if-else would require another variable
				switch(operatorStack.pop())
				{
					case "++":
						valueStack.push(valueStack.pop() + 1));
						break;
					case "--":
						valueStack.push(valueStack.pop() - 1));
						break;
					case "+":
						valueStack.push(valueStack.pop() + valueStack.pop());
						break;
					case "-":
						Integer second = valueStack.pop();
						valueStack.push( valueStack.pop() - second);
						break;
					case "*":
						valueStack.push(valueStack.pop() * valueStack.pop());
						break;
					case "/":
						Integer second = valueStack.pop();
						valueStack.push(valueStack.pop() / second);
						break;
					default:
						System.out.println("Illegal operator");
				} //end of case

			} // end of else-if
		} // end of for
		return valueStack.pop();
		//ADD YOUR CODE ABOVE HERE

		return null;   // DELETE THIS LINE
	}

	//Helper methods
	/**
	 * Helper method to test if a string is an integer
	 * Returns true for strings of integers like "456"
	 * and false for string of non-integers like "+"
	 * - DO NOT EDIT THIS METHOD
	 */
	private boolean isInteger(String element){
		try{
			Integer.valueOf(element);
		}catch(NumberFormatException e){
			return false;
		}
		return true;
	}

	/**
	 * Method to help print out the expression stored as a list in tokenList.
	 * - DO NOT EDIT THIS METHOD    
	 */

	@Override
	public String toString(){	
		String s = new String(); 
		for (String t : tokenList )
			s = s + "~"+  t;
		return s;		
	}

}


//package assignments2017.A2PostedCode;

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
		for(int i= 0; i<expressionString.length();i++)
		{
			//Check if string is not whitespace (since we ignore whitespaces)
			if(expressionString.charAt(i) != ' ')
			{
			//Check if there are invalid characters in the string 
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
					//else if here?
					else if(expressionString.charAt(i+1) == '-' || expressionString.charAt(i+1) == '*' || expressionString.charAt(i+1) == '/')
					{
						throw new IllegalArgumentException();
					}
				}
				if(expressionString.charAt(i) =='-')
				{
					if(expressionString.charAt(i+1) == '-')
					{
						token.append(expressionString.charAt(++i));
					}
					else if(expressionString.charAt(i+1) == '+' || expressionString.charAt(i+1) == '*' || expressionString.charAt(i+1) == '/')
					{
						throw new IllegalArgumentException();
					}
				}
				// Convert to string and add to arraylist and empty the token
				tokenList.add(token.toString());
				token.delete(0,token.length());
			} // end of if

		} //end of for
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
		// Push  2, +, 3, +, ++, 3 into either of two stacks, then when ) is seen,
		// evaluate ++3, pop from stack, store 4 on stack. For next ), evaluate 2+3+4 
		// and pop from stack, push 9 to the value stack and return 9 by poping the only element left
		// Reference: Please note I used the same form as Dijkstra's Shunting Yard Algorithm, which I saw in a 
		// class once.
		for(int i = 0;i < this.tokenList.size();i++)
		{
			// this stack of if-else statements ignores the opening [
			// and stores the closing ] as a string "abs" on the operatorStack
			// It also ignores the ( and { opening brackets
			if (this.isInteger(tokenList.get(i)))
			{
				//Push value to valueStack
				valueStack.push(Integer.parseInt(this.tokenList.get(i)));
			}
			else if(this.tokenList.get(i).equals("+") || this.tokenList.get(i).equals("-") || this.tokenList.get(i).equals("*") || this.tokenList.get(i).equals("/") || this.tokenList.get(i).equals("++") || this.tokenList.get(i).equals("--"))
			{
				// operators pushed to operatorStack
				operatorStack.push(this.tokenList.get(i));
			}
			else if(this.tokenList.get(i).equals("]"))
			{
				// Store the [] operator as abs when the closing bracket is reached
				operatorStack.push("abs");
			}
			
			else if (this.tokenList.get(i).equals(")"))
			{
				// Evaluate the last operation
				// Pop element from operatorStack for evaluation
				String operator = operatorStack.pop();
				if(operator.equals("++"))
					valueStack.push(valueStack.pop() + 1);
				else if (operator.equals("--"))
					valueStack.push(valueStack.pop() - 1);
				else if (operator.equals("+"))
					valueStack.push(valueStack.pop() + valueStack.pop());
				else if (operator.equals("-"))
				{
					//maintain order of subtraction
					// in 2 - 5, 5 will be pushed last and popped first
					Integer second = valueStack.pop();
					valueStack.push( valueStack.pop() - second);
				}
				else if (operator.equals("*"))
				{
					valueStack.push(valueStack.pop() * valueStack.pop());
				}
				else if (operator.equals("/"))
				{
						//maintain order of division
						Integer second = valueStack.pop();
						if(second != 0)
						{
							valueStack.push(valueStack.pop() / second);
						}
				}
				else if (operator.equals("abs"))
				{
					valueStack.push(Math.abs(valueStack.pop()));
				}
				else 
				{	//Illegal operator
					System.out.println("Illegal operator");
				} //end of evaluation else-if



			} //end of token else if
		} // end of for
		return valueStack.pop();
		//ADD YOUR CODE ABOVE HERE

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


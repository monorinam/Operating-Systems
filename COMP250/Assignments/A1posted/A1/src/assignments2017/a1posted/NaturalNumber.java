package assignments2017.a1posted;

/*
 *   STUDENT NAME      :  Monorina Mukhopadhyay	
 *   STUDENT ID        :  260364335
 *   
 *   If you have any issues that you wish the T.A.s to consider, then you
 *   should list them here.   If you discussed on the assignment in depth 
 *   with another student, then you should list that student's name here.   
 *   We insist that you each write your own code.   But we also expect 
 *   (and indeed encourage) that you discuss some of the technical
 *   issues and problems with each other, in case you get stuck.    

 *   
 */

import java.util.LinkedList;

public class NaturalNumber  {

	/*
	 *   
	 *  If the list has N coefficients,  then then the number is represented is a polynomial:
	 *  coefficients[N-1] base^{N-1} + ...  coefficients[1] base^{1} +  coefficients[0] 
	 *  where base has a particular value and the coefficients are in {0, 1, ...  base - 1}
	 *  
	 *  For any base and any positive integer, the representation of that positive 
	 *  integer as a sum of powers of that base is unique.  
	 *  
	 *  We require that the coefficient of the largest power is non-zero.  
	 *  For example,  '354' is a valid representation (which we call "three hundred fifty four") 
	 *  but '0354' is not.  
	 * 
	 */
	
	private int	base;       

	private LinkedList<Integer>  coefficients;

	//  Constructors

	NaturalNumber(int base){
		
		//  If no string argument is given, then it constructs an empty list of coefficients.
		
		this.base = base;
		coefficients = new LinkedList<Integer>();
	}

	/*       
	 *   The constructor builds a LinkedList of Integers where the integers need to be in {0,1,2,3,4...,base-1}.
	 *   The string only represents a base 10 number when the base is given to be 10.
	 */
	NaturalNumber(String sBase,  int base) throws Exception{
		int i;
		this.base = base;
		coefficients = new LinkedList<Integer>();
		if ((base <2) || (base > 10)){
			System.out.println("constructor error:  base must be between 2 and 10");
			throw new Exception();
		}

		/*
		 *    The large integer inputs will be read in as strings with character digits.  
		 *    These characters will need to be converted to integers.   The characters are represented 
		 *    in ASCII.   See the decimal (dec) and character (char) values in 
		 *    http://www.asciitable.com/  ).   The ASCII value of symbol '0' is 48,  and the ASCII value 
		 *    of symbol '1' is 49, etc.  So for example to get the numerical value of '2', we subtract 
		 *    the character value of '0' (48) from the character value of '2' (50).	
		 */
			
		int l = sBase.length();
		for (int indx = 0; indx < l; indx++){  
			i = sBase.charAt(indx);
			if ( (i >= 48) && (i - 48 < base))                                                    				
				coefficients.addFirst( new Integer(i-48) );
			else{
				System.out.println("constructor error:  all coefficients should be non-negative and less than base");
				throw new Exception();
			}			
		}
	}
	
	/*
	 *   Construct a NaturalNumber object for a number that has just one digit in [0, base).  
	 *   
	 *   This constructor acts as a helper.  It is not called from the Tester class.
	 */

	NaturalNumber(int i,  int base) throws Exception{
		this.base = base;
		coefficients = new LinkedList<Integer>();
		
		if ((i >= 0) && (i < base))
			coefficients.addFirst( new Integer(i) );
		else {
			System.out.println("constructor error: all coefficients should be non-negative and less than base");
			throw new Exception();
		}
	}

	/*
	 *   The plus method computes this.plus(b), that is, a+b where 'this' is a.
	 *   
	 *   If you do not know what the Java keyword 'this' is,  then see
	 *   https://docs.oracle.com/javase/tutorial/java/javaOO/thiskey.html
	 *	
	 */
	
	/* 
	 *	Getter for both the private variables- Added by Navin/Ramchalam for testGrader
	 */
	
	public int getBase()
	{
		return base;
	}
	public LinkedList<Integer>  getCoefficients()
	{
		return coefficients;
	}
	
	
	
	//To perform a+b, call a.plus(b). The parameter second refers to the second operand in a+b, that is, b
	
	public NaturalNumber plus( NaturalNumber  second) throws Exception{
				
		//  initialize the sum as an empty list of coefficients
		
		NaturalNumber sum = new NaturalNumber( this.base );

		if (this.base != second.base){
			System.out.println("ERROR: bases must be the same in an addition");
			throw new Exception();
		}

		/*   
		 * The plus method must not affect the numbers themselves. 
		 * So let's just work  with a copy (a clone) of the numbers. 
		 */

		NaturalNumber  firstClone  = this.clone();
		NaturalNumber  secondClone = second.clone();

		/*
		 *   If the two numbers have a different polynomial order    
		 *   then pad the smaller one with zero coefficients.
		 */
		
		int   diff = firstClone.coefficients.size() - second.coefficients.size();
		while (diff < 0){  // second is bigger
                                                     		
			firstClone.coefficients.addLast(0);			        
			diff++;
		}
		while (diff > 0){  //  this is bigger
			secondClone.coefficients.addLast(0);
			diff--;
		}

		//   ADD YOUR CODE HERE
		
		//  ---------  BEGIN SOLUTION (plus)  ----------
		//

		//   Note 'firstClone' and 'secondClone' have the same size.  We add the coefficients
		//   term by term.    If the last coefficient yields a carry, then we add 
		//   one more term with the carry.
		int carry = 0;
		for (int i = 0; i < firstClone.coefficients.size(); i++)
		{
			sum.coefficients.addLast((firstClone.coefficients.get(i) + secondClone.coefficients.get(i) + carry)%firstClone.base);
			carry = (firstClone.coefficients.get(i) + secondClone.coefficients.get(i) + carry)/firstClone.base;			
			
		}
		sum.coefficients.addLast(carry);
		sum = sum.leadZeroRemoval();
		//  ---------  END SOLUTION (plus)  ----------
		
		return sum;		
	}
	

	/*   
	 *    Slow multiplication algorithm, mentioned in lecture 1.
	 *    You need to implement the plus algorithm in order for this to work.
	 *    
	 *    'this' is the multiplicand i.e. a*b = a+a+a+...+a (b times) where a is multiplicand and b is multiplier
	 */
	
	public NaturalNumber slowTimes( NaturalNumber  multiplier) throws Exception{

		NaturalNumber prod  = new NaturalNumber(0, this.base);
		NaturalNumber one   = new NaturalNumber(1, this.base);
		for (NaturalNumber counter = new NaturalNumber(0, this.base);  counter.compareTo(multiplier) < 0;  counter = counter.plus(one) ){
			prod = prod.plus(this);
		}
		return prod;
	}
	
	/*
	 *    The multiply method must NOT be the same as what you learned in grade school since 
	 *    that method uses a temporary 2D table with space proportional to the square of 
	 *    the number of coefficients in the operands i.e. O(N^2).   Instead, you must write a method 
	 *    that uses space that is proportional to the number of coefficients i.e. O(N).    
	 *    Your algorithm will still take time O(N^2) however.   
	 */

	/*   The multiply method computes this.multiply(b) where 'this' is a.
	 */
	
	public NaturalNumber times( NaturalNumber multiplicand) throws Exception{
		
		//  initialize product as an empty list of coefficients
		
		NaturalNumber product	= new NaturalNumber( this.base );

		if (this.base != multiplicand.base){
			System.out.println("ERROR: bases must be the same in a multiplication");
			throw new Exception();
		}
		
		
		//    ADD YOUR CODE HERE
		
		// --------------  BEGIN SOLUTION (multiply)  ------------------
		
		/*
		 *           multiplicand
		 *          x  multiplier  (this)
		 *        ---------------
		 *            
		 *   Note we use a helper method.  See below.
		 */
		 NaturalNumber multiplier = this.clone();
		NaturalNumber multiplic2 = multiplicand.clone();
		NaturalNumber prodLine = new NaturalNumber( multiplier.base);
		/* Here, we calculate each line of the multiplication matrix using
		the timesSingleDigit helper method, and then add trailing zeros
		to the line, and finally add the line with zeros to the total to 
		get the answers. (So 234 * 25 gives us the first line as 1170, and no zeros are added, 
		and 468 as the second line so we make it 4680 and then add them. )
		*/
		for (int i = 0; i < multiplier.coefficients.size(); i++)
		{
			prodLine = multiplic2.timesSingleDigit(multiplier.coefficients.get(i));
			// pad with trailing zeros for the multiplication
			for(int j = 0; j < i; j ++)
			{
				prodLine.coefficients.addFirst(0);
			}
			product = product.plus(prodLine);
		}

		
		//  ---------------  END SOLUTION  (multiply) -------------------
		
		return product;
	}


	// -------- BEGIN SOLUTION     *helper method* for multiply  -----
	
	/*
	 *    'this' (the caller) will be the multiplicand.   
	 */
	/* This method implements the multiplication of a long number with a single digit using
	grade school arithmatic. We just multiply each coefficient with the factor and add any carry 
	from the previous line and then normalize using base to get the new carry*/
	public NaturalNumber timesSingleDigit(int factorMult)
	{
		NaturalNumber prodLine = new NaturalNumber(this.base);
		NaturalNumber mult1Clone = this.clone();
		int carry = 0;
		for (int i =0; i < mult1Clone.coefficients.size(); i++)
		{
			prodLine.coefficients.addLast((mult1Clone.coefficients.get(i)*factorMult + carry)%mult1Clone.base);
			carry = (mult1Clone.coefficients.get(i)*factorMult + carry)/mult1Clone.base;
			
		}
		prodLine.coefficients.addLast(carry);
		return prodLine.leadZeroRemoval();
	}
	
	
	//   END SOLUTION ----------  *helper method* for multiply ---------
	
	
	/*
	 *   The minus method computes this.minus(b) where 'this' is a, and a > b.
	 *   If a < b, then it throws an exception.
	 *	
	 */
	
	public NaturalNumber  minus(NaturalNumber second) throws Exception{

		//  initialize the result (difference) as an empty list of coefficients
		
		NaturalNumber  difference = new NaturalNumber(this.base);

		if (this.base != second.base){
			System.out.println("ERROR: bases must be the same in a subtraction");
			throw new Exception();
		}
		/*
		 *    The minus method is not supposed to change the numbers. 
		 *    But the grade school algorithm sometimes requires us to "borrow"
		 *    from a higher coefficient to a lower one.   So we work
		 *    with a copy (a clone) instead.
		 */

		NaturalNumber  first = this.clone();

		//   You may assume 'this' > second. 
		 
		if (this.compareTo(second) < 0){
			System.out.println("Error: the subtraction a-b requires that a > b");
			throw new Exception();
		}

		//   ADD YOUR CODE HERE
		
		//  ---------  BEGIN SOLUTION (minus)  ----------
		/*In this algorithm, first leading zeros are added to make the two numbers have the same numer of
		coefficients. Then I subtract the coefficients of second from first, starting at the 0th one. I track
		borrow using the borrow flag. The algorithm is like this:
		if previous line borrowed
			subtract one from first element
		if first coefficient is bigger than second or equal
			subtract them and set the borrow flag to 0
		if first coefficient is smaller
			set borrow flag to 1 and add base (the borrowing amount) 
			to second coefficient
		*/
		// Add leading zeros to clone of second to make subtraction easier
		NaturalNumber secondClone = second.clone();
		// Add leading zeros
		for(int i = 0; i < first.coefficients.size()- second.coefficients.size(); i++)
		{
			secondClone.coefficients.addLast(0);
		}
		// borrow is the flag which indicates if the previous elements borrowed from the current (1 is true)
		int borrow = 0;
		for(int i = 0; i < first.coefficients.size(); i++)
		{
			if(borrow == 1) // the last row of coefficients borrow from this row
			{
				first.coefficients.set(i,first.coefficients.get(i) - 1);
			}
			if(first.coefficients.get(i) >= secondClone.coefficients.get(i))
			{	
				difference.coefficients.addLast(first.coefficients.get(i) - secondClone.coefficients.get(i));
				borrow = 0;
			}
			else
			{
				borrow = 1;
				difference.coefficients.addLast(first.base + first.coefficients.get(i) - secondClone.coefficients.get(i));
			}
		}	

		//  ---------  END SOLUTION (minus)  ----------

		
		/*  
		 *  In the case of say  100-98, we will end up with 002.  
		 *  Remove all the leading 0's of the result.
		 *
		 *  We are giving you this code because you could easily neglect
		 *  to do this check, which would mess up grading since correct
		 *  answers would appear incorrect.
		 */
		
		while ((difference.coefficients.size() > 1) & 
				(difference.coefficients.getLast().intValue() == 0)){
			difference.coefficients.removeLast();
		}
		return difference;	
	}

	/*   
	 *    Slow division algorithm, mentioned in lecture 1.
	 */

	
	public NaturalNumber slowDivide( NaturalNumber  divisor) throws Exception{

		NaturalNumber quotient = new NaturalNumber(0,base);
		NaturalNumber one = new NaturalNumber(1,base);
		NaturalNumber remainder = this.clone();
		while ( remainder.compareTo(divisor) >= 0 ){
			remainder = remainder.minus(divisor);
			quotient = quotient.plus(one);
		}
		return quotient;
	}


	
	/*  
	 *  The divide method divides 'this' by 'divisor' i.e. this.divide(divisor)
	 *   When this method terminates, there is a remainder but it is ignored (not returned).
	 *   
	 */
	
	public NaturalNumber divide( NaturalNumber  divisor ) throws Exception{
		
		//  initialize quotient as an empty list of coefficients
		
		NaturalNumber  quotient = new NaturalNumber(this.base);
		
		if (this.base != divisor.base){
			System.out.println("ERROR: bases must be the same in an division");
			throw new Exception();
		}
		
		if(divisor.compareTo(new NaturalNumber(0, this.base))==0){
			System.out.println("ERROR: division by zero not possible");
			throw new Exception();
		}
		
		NaturalNumber  remainder = this.clone();
		

		
		

		//   ADD YOUR CODE HERE.
		
		//  --------------- BEGIN SOLUTION (divide) --------------------------
		/* Long division algorithm:
		The algorithm implemented is a version of the long division algorithm
		called Chunking: https://en.wikipedia.org/wiki/Chunking_(division)

		I also referenced the binary version here: 
		https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_.28unsigned.29_with_remainder 
		Shifting an element in binary is the same as doing element * base^factor in any other base

		In long divisor, say with 3642 divided by 14, what we do is:
		 we 'pick' 36 and find what 36/14 is (=2), and then subtract 14*2 from 
		36 to get the remainder for the next line, and then bring down the next number (4) 
		from 3642. Technically, this is doing 3642 - 1400*2 = 842 and then 842 - 140*6 and so on
		until we get a remainder less than 14. 
		The algorithm is 
		while(remainder > divisor)
			factor = sizeOf(remainder) - sizeOf(divisor)
			divisorMult = divisor * base^(factor)
			quotientChunk = 0
			while(remainder > divisorMult*quotientChunk)
				quotientChunk ++
			remainder = remainder - divisorMult*(quotientChunk)
			update quotient as quotient + [quotientChunk 0 0 .. factor-1 times] 
		*/
		

		//Some exceptional cases to take care of:
		if(remainder.compareTo(divisor) == 0) //(a / a should give us 1)
			quotient.coefficients.addFirst(1);
		else
			quotient.coefficients.addFirst(0); //to make sure that if the a/b with a < b then zero is returned.


		while(remainder.compareTo(divisor) == 1)
		{
			//Find the multiple of the divisor to create the chunk 
			// First find the multiples of base to scale the divisor by
			int factor = remainder.coefficients.size() - divisor.coefficients.size();
			// To handle the case where divisor*(base^factor) would be greater than remainder 
			// Such as 358 / 9 and then factor would give 2 and 900 > 358 so we have to
			// make that chunk 90
			if(divisor.clone().timesBaseToThePower(factor).compareTo(remainder) == 1)
			{
				factor = factor - 1;
				
			}
			// this is a faster way that looping through and adding zeros but they do the same
			// (In the binary algorithm, this is "shifting" to the left)
			NaturalNumber divisorMult = divisor.clone().timesBaseToThePower(factor);
			
			NaturalNumber quotientChunk = new NaturalNumber(this.base);
			quotientChunk.coefficients.addFirst(0);

			// this part replaces slow division by finding the multiple of the divisor chunk that needs
			// to be subtracted from remainder so for 487/2, the previous part makes 200 and this 
			// part finds the scale to turn 200 to 400, and then
			// subtracting divisor * multiple to find the quotient

			while(remainder.compareTo(divisorMult.timesSingleDigit(quotientChunk.coefficients.getFirst() + 1)) == 1)
			{
				quotientChunk.coefficients.set(0, quotientChunk.coefficients.get(0) + 1);
			}
			
			remainder = remainder.minus(divisorMult.timesSingleDigit(quotientChunk.coefficients.get(0)));
			//Update the quotient. Adding to the previous quotient takes care of cases
			// With zeros in between. So if 809/8, then the first chunk is 800 (100*8), remainder
			// is 8, and we need to save the quotient as 100 and not just 1. In the next round,
			// we want 100 + 1, not 1001 so we add to previous quotient instead of just doing addFirst().
			for (int i = 1; i <= factor ; i++)
			{
					quotientChunk.coefficients.addFirst(0);
			}
			quotient = quotient.plus(quotientChunk);

			// store the divisor here again
		}
		// remove any leading zeros
		quotient = quotient.leadZeroRemoval();
		// -------------  END SOLUTION  (divide)  ---------------------

		return quotient;		
	}
	
	//   Helper methods

	/*
	 * The methods you write to add, subtract, multiply, divide 
	 * should not alter the two numbers.  If a method require
	 * that one of the numbers be altered (e.g. borrowing in subtraction)
	 * then you need to clone the number and work with the cloned number 
	 * instead of the original. 
	 */
	
	@Override
	public NaturalNumber  clone(){

		//  For technical reasons that don't interest us now (and perhaps ever), this method 
		//  has to be declared public (not private).

		NaturalNumber copy = new NaturalNumber(this.base);
		for (int i=0; i < this.coefficients.size(); i++){
			copy.coefficients.addLast( new Integer( this.coefficients.get(i) ) );
		}
		return copy;
	}

	public NaturalNumber leadZeroRemoval()
	{
		// This method removes leading zeros from a number.
		// It is the exact same thing as from the minus method
		// given in the handout
		NaturalNumber cloneNum = this.clone();
		while ((cloneNum.coefficients.size() > 1) & 
				(cloneNum.coefficients.getLast().intValue() == 0)){
			cloneNum.coefficients.removeLast();
		}
		return cloneNum;	
	}
	/*
	 *  The subtraction method (minus) computes a-b and requires that a>b.   
	 *  The a.compareTo(b) method is useful for checking this condition. 
	 *  It returns -1 if a < b,  it returns 0 if a == b,  
	 *  and it returns 1 if a > b.
	 *  
	 *  The compareTo() method assumes that the two numbers have the same base.
	 *  One could add code to check this but I didn't.
	 */
	private int 	compareTo(NaturalNumber second){

		//   if  this < other,  return -1  
		//   if  this > other,  return  1  
		//   otherwise they are equal and return 0
		
		//   Assume maximum degree coefficient is non-zero.   Then,  if two numbers
		//   have different maximum degree, it is easy to decide which is larger.
		
		int diff = this.coefficients.size() - second.coefficients.size();
		if (diff < 0)
			return -1;
		else if (diff > 0)
			return 1;
		else { 
			
			//   If two numbers have the same maximum degree,  then it is a bit trickier
			//   to decide which number is larger.   You need to compare the coefficients,
			//   starting from the largest and working toward the smallest until you find
			//   coefficients that are not equal.
			
			boolean done = false;
			int i = this.coefficients.size() - 1;
			while (i >=0 && !done){
				diff = this.coefficients.get(i) - second.coefficients.get(i); 
				if (diff < 0){
					return -1;
				}
				else if (diff > 0)
					return 1;
				else{
					i--;
				}
			}
			return 0;    //   if all coefficients are the same,  so numbers are equal.
		}
	}

	/*  computes  'this' * base^n  
	 */
	
	private NaturalNumber timesBaseToThePower(int n){
		for (int i=0; i< n; i++){
			this.coefficients.addFirst(new Integer(0));
		}
		return this;
	}

	/*
	The following method is invoked by System.out.print.
	It returns the string with coefficients in the reverse order 
	which is the natural format for people to reading numbers,
	i.e. people want to read  a[N-1], ... a[2] a[1] a[0]. 
	It does so simply by make a copy of the list with elements in 
	reversed order, and then printing the list using the LinkedList's
	toString() method.
	*/
	
	@Override
	public String toString(){	
		String s = new String(); 
		for (Integer coef : coefficients)     //  Java enhanced for loop
			s = coef.toString() + s ;        //   Append each successive coefficient.
		return "(" + s + ")_" + base;		
	}
	
}


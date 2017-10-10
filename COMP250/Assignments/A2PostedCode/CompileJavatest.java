import java.lang.Math; // headers MUST be above the first class
import java.util.ArrayList;

// one class needs to have a main() method
public class HelloWorld
{
  // arguments are passed using the text field below this editor
  public static void main(String[] args)
  {
    OtherClass myObject = new OtherClass("Hello World!");
    System.out.print(myObject);
    ArrayList<String> tokenList = new ArrayList<String>();
    StringBuilder token = new StringBuilder();
    String expressionString="(3 + 1 + (--2))";
    for(int i= 0; i<expressionString.length();i++)
        {
            //Check if string is whitespace, if so, ignore
            if(expressionString.charAt(i)==' ')
                continue;
            
            //Check if there are invalid characters in the string TODO
            
            token.append(expressionString.charAt(i));
            //If the element is a number, then need to store the full number.
            // not just an element of the number
            if(Character.isDigit(expressionString.charAt(i)))//.isDigit())
            {
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
            if(expressionString.charAt(i)=='-')
            {
                if(expressionString.charAt(i+1) == '-')
                {
                    token.append(expressionString.charAt(++i));
                }
            }
            System.out.println(token);
            tokenList.add(token.toString());
            token.delete(0,token.length());

        }
    System.out.println(tokenList);
  }
}

// you can add other public classes to this editor in any order
public class OtherClass
{
  private String message;
  private boolean answer = false;
  public OtherClass(String input)
  {
    message = "Why, " + input + " Isn't this something?";
  }
  public String toString()
  {
    return message;
  }
}
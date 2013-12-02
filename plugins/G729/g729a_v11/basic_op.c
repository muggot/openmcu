
/*
 *                                                                           
 * Basics operators.                                                         
 *
*/

/*
 *                                                                           
 *   Include-Files                                                           
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"


/*
 *                                                                           
 *   Function Name : sature                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Limit the 32 bit input to the range of a 16 bit word.                  
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
sature (Word32 L_var1)
{
  Word16 var_out;

  if (L_var1 > 0X00007fffL) {
    var_out = MAX_16;
  }
  else if (L_var1 < (Word32) 0xffff8000L) {
    var_out = MIN_16;
  }
  else {
    var_out = extract_l (L_var1);
  }

  return (var_out);
}


/*
 *                                                                           
 *   Function Name : sature                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Limit the 32 bit input to the range of a 16 bit word.                  
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
sature_o (Word32 L_var1, Flag *Overflow)
{
  Word16 var_out;

  if (L_var1 > 0X00007fffL) {
    *Overflow = 1;
    var_out = MAX_16;
  }
  else if (L_var1 < (Word32) 0xffff8000L) {
    *Overflow = 1;
    var_out = MIN_16;
  }
  else {
    *Overflow = 0;
    var_out = extract_l (L_var1);
  }

  return (var_out);
}

/*
 *                                                                           
 *   Function Name : add                                                     
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Performs the addition (var1+var2) with overflow control and saturation;
 *    the 16 bit result is set at +32767 when overflow occurs or at -32768   
 *    when underflow occurs.                                                 
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
add (Word16 var1, Word16 var2)
{
  Word16 var_out;
  Word32 L_somme;

  L_somme = (Word32) var1 + var2;
  var_out = sature (L_somme);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : sub                                                     
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Performs the subtraction (var1+var2) with overflow control and satu-   
 *    ration; the 16 bit result is set at +32767 when overflow occurs or at  
 *    -32768 when underflow occurs.                                          
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
sub (Word16 var1, Word16 var2)
{
  Word16 var_out;
  Word32 L_diff;

  L_diff = (Word32) var1 - var2;
  var_out = sature (L_diff);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : add_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Performs the addition (var1+var2) with overflow control and saturation;
 *    the 16 bit result is set at +32767 when overflow occurs or at -32768   
 *    when underflow occurs.                                                 
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
add_o (Word16 var1, Word16 var2, Flag *Overflow)
{
  Word16 var_out;
  Word32 L_somme;

  L_somme = (Word32) var1 + var2;
  var_out = sature_o (L_somme, Overflow);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : sub_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Performs the subtraction (var1+var2) with overflow control and satu-   
 *    ration; the 16 bit result is set at +32767 when overflow occurs or at  
 *    -32768 when underflow occurs.                                          
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
sub_o (Word16 var1, Word16 var2, Flag *Overflow)
{
  Word16 var_out;
  Word32 L_diff;

  L_diff = (Word32) var1 - var2;
  var_out = sature_o (L_diff, Overflow);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : abs_s                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Absolute value of var1; abs_s(-32768) = 32767.                         
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
abs_s (Word16 var1)
{
  Word16 var_out;

  if (var1 == (Word16) 0X8000) {
    var_out = MAX_16;
  }
  else {
    if (var1 < 0) {
      var_out = -var1;
    }
    else {
      var_out = var1;
    }
  }
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : shl                                                     
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Arithmetically shift the 16 bit input var1 left var2 positions.Zero fill
 *   the var2 LSB of the result. If var2 is negative, arithmetically shift   
 *   var1 right by -var2 with sign extension. Saturate the result in case of 
 *   underflows or overflows.                                                
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
shl (Word16 var1, Word16 var2)
{
  Word16 var_out;
  Word32 resultat;

  if (var2 < 0) {
    var_out = shr (var1, -var2);
  }
  else {
    resultat = (Word32) var1 *((Word32) 1 << var2);
    if ((var2 > 15 && var1 != 0)
	|| (resultat != (Word32) ((Word16) resultat))) {
      var_out = (var1 > 0) ? MAX_16 : MIN_16;
    }
    else {
      var_out = extract_l (resultat);
    }
  }
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : shr                                                     
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Arithmetically shift the 16 bit input var1 right var2 positions with    
 *   sign extension. If var2 is negative, arithmetically shift var1 left by  
 *   -var2 with sign extension. Saturate the result in case of underflows or 
 *   overflows.                                                              
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
shr (Word16 var1, Word16 var2)
{
  Word16 var_out;

  if (var2 < 0) {
    var_out = shl (var1, -var2);
  }
  else {
    if (var2 >= 15) {
      var_out = (var1 < 0) ? (Word16) (-1) : (Word16) 0;
    }
    else {
      if (var1 < 0) {
	var_out = ~((~var1) >> var2);
      }
      else {
	var_out = var1 >> var2;
      }
    }
  }

  return (var_out);
}

/*
 *                                                                           
 *   Function Name : mult                                                    
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Performs the multiplication of var1 by var2 and gives a 16 bit result  
 *    which is scaled i.e.:                                                  
 *             mult(var1,var2) = shr((var1 times var2),15) and               
 *             mult(-32768,-32768) = 32767.                                  
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
mult (Word16 var1, Word16 var2)
{
  Word16 var_out;
  Word32 L_produit;

  L_produit = (Word32) var1 *(Word32) var2;

  L_produit = (L_produit & (Word32) 0xffff8000L) >> 15;

  if (L_produit & (Word32) 0x00010000L)
    L_produit = L_produit | (Word32) 0xffff0000L;

  var_out = sature (L_produit);
  return (var_out);
}


/*
 *                                                                           
 *   Function Name : L_mult                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   L_mult is the 32 bit result of the multiplication of var1 times var2    
 *   with one shift left i.e.:                                               
 *        L_mult(var1,var2) = shl((var1 times var2),1) and                   
 *        L_mult(-32768,-32768) = 2147483647.                                
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_mult (Word16 var1, Word16 var2)
{
  Word32 L_var_out;

  L_var_out = (Word32) var1 *(Word32) var2;
  if (L_var_out != (Word32) 0x40000000L) {
    L_var_out *= 2;
  }
  else {
    L_var_out = MAX_32;
  }

  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_mult_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   L_mult is the 32 bit result of the multiplication of var1 times var2    
 *   with one shift left i.e.:                                               
 *        L_mult(var1,var2) = shl((var1 times var2),1) and                   
 *        L_mult(-32768,-32768) = 2147483647.                                
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_mult_o (Word16 var1, Word16 var2, Flag *Overflow)
{
  Word32 L_var_out;

  L_var_out = (Word32) var1 *(Word32) var2;
  if (L_var_out != (Word32) 0x40000000L) {
    L_var_out *= 2;
  }
  else {
    *Overflow = 1;
    L_var_out = MAX_32;
  }

  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : negate                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Negate var1 with saturation, saturate in the case where input is -32768:
 *                negate(var1) = sub(0,var1).                                
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
negate (Word16 var1)
{
  Word16 var_out;

  var_out = (var1 == MIN_16) ? MAX_16 : -var1;
  return (var_out);
}


/*
 *                                                                           
 *   Function Name : extract_h                                               
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Return the 16 MSB of L_var1.                                            
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32 ) whose value falls in the 
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
extract_h (Word32 L_var1)
{
  Word16 var_out;

  var_out = (Word16) (L_var1 >> 16);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : extract_l                                               
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Return the 16 LSB of L_var1.                                            
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32 ) whose value falls in the 
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
extract_l (Word32 L_var1)
{
  Word16 var_out;

  var_out = (Word16) L_var1;
  return (var_out);
}


/*
 *                                                                           
 *   Function Name : wround                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Round the lower 16 bits of the 32 bit input number into its MS 16 bits  
 *   with saturation. Shift the resulting bits right by 16 and return the 16 
 *   bit number:                                                             
 *               wround(L_var1) = extract_h(L_add(L_var1,32768))              
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32 ) whose value falls in the 
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
wround (Word32 L_var1)
{
  Word16 var_out;
  Word32 L_arrondi;

  L_arrondi = L_add (L_var1, (Word32) 0x00008000);
  var_out = extract_h (L_arrondi);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : wround_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Round the lower 16 bits of the 32 bit input number into its MS 16 bits  
 *   with saturation. Shift the resulting bits right by 16 and return the 16 
 *   bit number:                                                             
 *               wround(L_var1) = extract_h(L_add(L_var1,32768))              
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32 ) whose value falls in the 
 *             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
wround_o (Word32 L_var1, Flag *Overflow)
{
  Word16 var_out;
  Word32 L_arrondi;

  L_arrondi = L_add_o (L_var1, (Word32) 0x00008000, Overflow);
  var_out = extract_h (L_arrondi);
  return (var_out);
}


/*
 *                                                                           
 *   Function Name : L_mac                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    
 *   result to L_var3 with saturation, return a 32 bit result:               
 *        L_mac(L_var3,var1,var2) = L_add(L_var3,(L_mult(var1,var2)).        
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_mac (Word32 L_var3, Word16 var1, Word16 var2)
{
  Word32 L_var_out;
  Word32 L_produit;

  L_produit = L_mult (var1, var2);
  L_var_out = L_add (L_var3, L_produit);
  return (L_var_out);
}


/*
 *                                                                           
 *   Function Name : L_mac_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    
 *   result to L_var3 with saturation, return a 32 bit result:               
 *        L_mac(L_var3,var1,var2) = L_add(L_var3,(L_mult(var1,var2)).        
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_mac_o (Word32 L_var3, Word16 var1, Word16 var2, Flag* Overflow)
{
  Word32 L_var_out;
  Word32 L_produit;

  L_produit = L_mult_o (var1, var2, Overflow);
  L_var_out = L_add_o (L_var3, L_produit, Overflow);
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_msu                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   
 *   bit result to L_var3 with saturation, return a 32 bit result:           
 *        L_msu(L_var3,var1,var2) = L_sub(L_var3,(L_mult(var1,var2)).        
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_msu (Word32 L_var3, Word16 var1, Word16 var2)
{
  Word32 L_var_out;
  Word32 L_produit;

  L_produit = L_mult (var1, var2);
  L_var_out = L_sub (L_var3, L_produit);
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_msu_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   
 *   bit result to L_var3 with saturation, return a 32 bit result:           
 *        L_msu(L_var3,var1,var2) = L_sub(L_var3,(L_mult(var1,var2)).        
 *                                                                           
 *   Complexity weight : 1                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_msu_o (Word32 L_var3, Word16 var1, Word16 var2, Flag *Overflow)
{
  Word32 L_var_out;
  Word32 L_produit;

  L_produit = L_mult_o (var1, var2, Overflow);
  L_var_out = L_sub_o (L_var3, L_produit, Overflow);
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_add                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      
 *   overflow control and saturation; the result is set at +214783647 when   
 *   overflow occurs or at -214783648 when underflow occurs.                 
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    L_var2   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_add (Word32 L_var1, Word32 L_var2)
{
  Word32 L_var_out;

  L_var_out = L_var1 + L_var2;

  if (((L_var1 ^ L_var2) & MIN_32) == 0) {
    if ((L_var_out ^ L_var1) & MIN_32) {
      L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_sub                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   
 *   overflow control and saturation; the result is set at +214783647 when   
 *   overflow occurs or at -214783648 when underflow occurs.                 
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    L_var2   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_sub (Word32 L_var1, Word32 L_var2)
{
  Word32 L_var_out;

  L_var_out = L_var1 - L_var2;

  if (((L_var1 ^ L_var2) & MIN_32) != 0) {
    if ((L_var_out ^ L_var1) & MIN_32) {
      L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_add_o                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      
 *   overflow control and saturation; the result is set at +214783647 when   
 *   overflow occurs or at -214783648 when underflow occurs.                 
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    L_var2   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_add_o (Word32 L_var1, Word32 L_var2, Flag *Overflow)
{
  Word32 L_var_out;

  L_var_out = L_var1 + L_var2;

  if (((L_var1 ^ L_var2) & MIN_32) == 0) {
    if ((L_var_out ^ L_var1) & MIN_32) {
      L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
      *Overflow = 1;
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_sub_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   
 *   overflow control and saturation; the result is set at +214783647 when   
 *   overflow occurs or at -214783648 when underflow occurs.                 
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    L_var2   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_sub_o (Word32 L_var1, Word32 L_var2, Flag *Overflow)
{
  Word32 L_var_out;

  L_var_out = L_var1 - L_var2;

  if (((L_var1 ^ L_var2) & MIN_32) != 0) {
    if ((L_var_out ^ L_var1) & MIN_32) {
      L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
      *Overflow = 1;
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_negate                                                
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Negate the 32 bit variable L_var1 with saturation; saturate in the case 
 *   where input is -2147483648 (0x8000 0000).                               
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_negate (Word32 L_var1)
{
  Word32 L_var_out;

  L_var_out = (L_var1 == MIN_32) ? MAX_32 : -L_var1;
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : mult_r                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Same as mult with rounding, i.e.:                                       
 *     mult_r(var1,var2) = shr(((var1*var2) + 16384),15) and                 
 *     mult_r(-32768,-32768) = 32767.                                        
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
mult_r (Word16 var1, Word16 var2)
{
  Word16 var_out;
  Word32 L_produit_arr;

  L_produit_arr = (Word32) var1 *(Word32) var2;	/* product */
  L_produit_arr += (Word32) 0x00004000;	/* round */
  L_produit_arr &= (Word32) 0xffff8000L;
  L_produit_arr >>= 15;		/* shift */

  if (L_produit_arr & (Word32) 0x00010000L) {	/* sign extend when necessary */
    L_produit_arr |= (Word32) 0xffff0000L;
  }

  var_out = sature (L_produit_arr);
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : L_shl                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  
 *   fill the var2 LSB of the result. If var2 is negative, L_var1 right by   
 *   -var2 arithmetically shift with sign extension. Saturate the result in  
 *   case of underflows or overflows.                                        
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_shl (Word32 L_var1, Word16 var2)
{
  Word32 L_var_out;

  /* initialization used only to suppress Microsoft Visual C++ warnings */
  L_var_out = 0L;

  if (var2 <= 0) {
    L_var_out = L_shr (L_var1, -var2);
  }
  else {
    for (; var2 > 0; var2--) {
      if (L_var1 > (Word32) 0X3fffffffL) {
	L_var_out = MAX_32;
	break;
      }
      else {
	if (L_var1 < (Word32) 0xc0000000L) {
	  L_var_out = MIN_32;
	  break;
	}
      }
      L_var1 *= 2;
      L_var_out = L_var1;
    }
  }
  return (L_var_out);
}


/*
 *                                                                           
 *   Function Name : L_shl_o
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  
 *   fill the var2 LSB of the result. If var2 is negative, L_var1 right by   
 *   -var2 arithmetically shift with sign extension. Saturate the result in  
 *   case of underflows or overflows.                                        
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_shl_o (Word32 L_var1, Word16 var2, Flag *Overflow)
{
  Word32 L_var_out;

  /* initialization used only to suppress Microsoft Visual C++ warnings */
  L_var_out = 0L;

  if (var2 <= 0) {
    L_var_out = L_shr (L_var1, -var2);
  }
  else {
    for (; var2 > 0; var2--) {
      if (L_var1 > (Word32) 0X3fffffffL) {
	*Overflow = 1;
	L_var_out = MAX_32;
	break;
      }
      else {
	if (L_var1 < (Word32) 0xc0000000L) {
	  *Overflow = 1;
	  L_var_out = MIN_32;
	  break;
	}
      }
      L_var1 *= 2;
      L_var_out = L_var1;
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_shr                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Arithmetically shift the 32 bit input L_var1 right var2 positions with  
 *   sign extension. If var2 is negative, arithmetically shift L_var1 left   
 *   by -var2 and zero fill the var2 LSB of the result. Saturate the result  
 *   in case of underflows or overflows.                                     
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              
 *
*/

Word32
L_shr (Word32 L_var1, Word16 var2)
{
  Word32 L_var_out;

  if (var2 < 0) {
    L_var_out = L_shl (L_var1, -var2);
  }
  else {
    if (var2 >= 31) {
      L_var_out = (L_var1 < 0L) ? -1 : 0;
    }
    else {
      if (L_var1 < 0) {
	L_var_out = ~((~L_var1) >> var2);
      }
      else {
	L_var_out = L_var1 >> var2;
      }
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : shr_r                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Same as shr(var1,var2) but with rounding. Saturate the result in case of
 *   underflows or overflows :                                               
 *    If var2 is greater than zero :                                         
 *       shr_r(var1,var2) = shr(add(var1,2**(var2-1)),var2)                  
 *    If var2 is less than zero :                                            
 *       shr_r(var1,var2) = shr(var1,var2).                                  
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word16
shr_r (Word16 var1, Word16 var2)
{
  Word16 var_out;

  if (var2 > 15) {
    var_out = 0;
  }
  else {
    var_out = shr (var1, var2);

    if (var2 > 0) {
      if ((var1 & ((Word16) 1 << (var2 - 1))) != 0) {
	var_out++;
      }
    }
  }
  return (var_out);
}

/*
 *                                                                           
 *   Function Name : mac_r                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    
 *   result to L_var3 with saturation. Round the LS 16 bits of the result    
 *   into the MS 16 bits with saturation and shift the result right by 16.   
 *   Return a 16 bit result.                                                 
 *            mac_r(L_var3,var1,var2) = wround(L_mac(Lvar3,var1,var2))        
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              
 *
*/

Word16
mac_r (Word32 L_var3, Word16 var1, Word16 var2)
{
  Word16 var_out;

  L_var3 = L_mac (L_var3, var1, var2);
  L_var3 = L_add (L_var3, (Word32) 0x00008000);
  var_out = extract_h (L_var3);
  return (var_out);
}


/*
 *                                                                           
 *   Function Name : msu_r                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   
 *   bit result to L_var3 with saturation. Round the LS 16 bits of the res-  
 *   ult into the MS 16 bits with saturation and shift the result right by   
 *   16. Return a 16 bit result.                                             
 *            msu_r(L_var3,var1,var2) = wround(L_msu(Lvar3,var1,var2))        
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var3   32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              
 *
*/

Word16
msu_r (Word32 L_var3, Word16 var1, Word16 var2)
{
  Word16 var_out;

  L_var3 = L_msu (L_var3, var1, var2);
  L_var3 = L_add (L_var3, (Word32) 0x00008000);
  var_out = extract_h (L_var3);
  return (var_out);
}



/*
 *                                                                           
 *   Function Name : L_deposit_h                                             
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Deposit the 16 bit var1 into the 16 MS bits of the 32 bit output. The   
 *   16 LS bits of the output are zeroed.                                    
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= var_out <= 0x7fff 0000.                
 *
*/

Word32
L_deposit_h (Word16 var1)
{
  Word32 L_var_out;

  L_var_out = (Word32) var1 << 16;
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_deposit_l                                             
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Deposit the 16 bit var1 into the 16 LS bits of the 32 bit output. The   
 *   16 MS bits of the output are sign extended.                             
 *                                                                           
 *   Complexity weight : 2                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0xFFFF 8000 <= var_out <= 0x0000 7fff.                
 *
*/

Word32
L_deposit_l (Word16 var1)
{
  Word32 L_var_out;

  L_var_out = (Word32) var1;
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_shr_r                                                 
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Same as L_shr(L_var1,var2)but with rounding. Saturate the result in case
 *   of underflows or overflows :                                            
 *    If var2 is greater than zero :                                         
 *       L_shr_r(var1,var2) = L_shr(L_add(L_var1,2**(var2-1)),var2)          
 *    If var2 is less than zero :                                            
 *       L_shr_r(var1,var2) = L_shr(L_var1,var2).                            
 *                                                                           
 *   Complexity weight : 3                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                
 *
*/

Word32
L_shr_r (Word32 L_var1, Word16 var2)
{
  Word32 L_var_out;

  if (var2 > 31) {
    L_var_out = 0;
  }
  else {
    L_var_out = L_shr (L_var1, var2);
    if (var2 > 0) {
      if ((L_var1 & ((Word32) 1 << (var2 - 1))) != 0) {
	L_var_out++;
      }
    }
  }
  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : L_abs                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *    Absolute value of L_var1; Saturate in case where the input is          
 *                                                               -214783648  
 *                                                                           
 *   Complexity weight : 3                                                   
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    L_var_out                                                              
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x0000 0000 <= var_out <= 0x7fff ffff.                
 *
*/

Word32
L_abs (Word32 L_var1)
{
  Word32 L_var_out;

  if (L_var1 == MIN_32) {
    L_var_out = MAX_32;
  }
  else {
    if (L_var1 < 0) {
      L_var_out = -L_var1;
    }
    else {
      L_var_out = L_var1;
    }
  }

  return (L_var_out);
}

/*
 *                                                                           
 *   Function Name : norm_s                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Produces the number of left shift needed to normalize the 16 bit varia- 
 *   ble var1 for positive values on the interval with minimum of 16384 and  
 *   maximum of 32767, and for negative values on the interval with minimum  
 *   of -32768 and maximum of -16384; in order to normalize the result, the  
 *   following operation must be done :                                      
 *                    norm_var1 = shl(var1,norm_s(var1)).                    
 *                                                                           
 *   Complexity weight : 15                                                  
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 0000 <= var_out <= 0x0000 000f.                
 *
*/

Word16
norm_s (Word16 var1)
{
  Word16 var_out;

  if (var1 == 0) {
    var_out = 0;
  }
  else {
    if (var1 == (Word16) 0xffff) {
      var_out = 15;
    }
    else {
      if (var1 < 0) {
	var1 = ~var1;
      }

      for (var_out = 0; var1 < 0x4000; var_out++) {
	var1 <<= 1;
      }
    }
  }

  return (var_out);
}


/*
 *                                                                           
 *   Function Name : div_s                                                   
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Produces a result which is the fractional  integer division of var1 by  
 *   var2; var1 and var2 must be positive and var2 must be greater or equal  
 *   to var1; the result is positive (leading bit equal to 0) and truncated  
 *   to 16 bits.                                                             
 *   If var1 = var2 then div(var1,var2) = 32767.                             
 *                                                                           
 *   Complexity weight : 18                                                  
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    var1                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 0000 <= var1 <= var2 and var2 != 0.            
 *                                                                           
 *    var2                                                                   
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : var1 <= var2 <= 0x0000 7fff and var2 != 0.            
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                
 *             It's a Q15 value (point between b15 and b14).                 
 *
*/

Word16
div_s (Word16 var1, Word16 var2)
{
  Word16 var_out = 0;
  Word16 iteration;
  Word32 L_num;
  Word32 L_denom;

  if ((var1 > var2) || (var1 < 0) || (var2 < 0)) {
    printf ("Division Error var1=%d  var2=%d\n", var1, var2);
    exit (0);
  }

  if (var2 == 0) {
    printf ("Division by 0, Fatal error \n");
    exit (0);
  }

  if (var1 == 0) {
    var_out = 0;
  }
  else {
    if (var1 == var2) {
      var_out = MAX_16;
    }
    else {
      L_num = L_deposit_l (var1);
      L_denom = L_deposit_l (var2);

      for (iteration = 0; iteration < 15; iteration++) {
	var_out <<= 1;
	L_num <<= 1;

	if (L_num >= L_denom) {
	  L_num = L_sub (L_num, L_denom);
	  var_out = add (var_out, 1);
	}
      }
    }
  }

  return (var_out);
}


/*
 *                                                                           
 *   Function Name : norm_l                                                  
 *                                                                           
 *   Purpose :                                                               
 *                                                                           
 *   Produces the number of left shift needed to normalize the 32 bit varia- 
 *   ble l_var1 for positive values on the interval with minimum of          
 *   1073741824 and maximum of 2147483647, and for negative values on the in-
 *   terval with minimum of -2147483648 and maximum of -1073741824; in order 
 *   to normalize the result, the following operation must be done :         
 *                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             
 *                                                                           
 *   Complexity weight : 30                                                  
 *                                                                           
 *   Inputs :                                                                
 *                                                                           
 *    L_var1                                                                 
 *             32 bit long signed integer (Word32) whose value falls in the  
 *             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   
 *                                                                           
 *   Outputs :                                                               
 *                                                                           
 *    none                                                                   
 *                                                                           
 *   Return Value :                                                          
 *                                                                           
 *    var_out                                                                
 *             16 bit short signed integer (Word16) whose value falls in the 
 *             range : 0x0000 0000 <= var_out <= 0x0000 001f.                
 *
*/

Word16
norm_l (Word32 L_var1)
{
  Word16 var_out;

  if (L_var1 == 0) {
    var_out = 0;
  }
  else {
    if (L_var1 == (Word32) 0xffffffffL) {
      var_out = 31;
    }
    else {
      if (L_var1 < 0) {
	L_var1 = ~L_var1;
      }

      for (var_out = 0; L_var1 < (Word32) 0x40000000L; var_out++) {
	L_var1 <<= 1;
      }
    }
  }

  return (var_out);
}

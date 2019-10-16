#include <stdio.h>
#include <math.h>
#include <stdlib.h> //if I need it
#include <string.h>
#include <inttypes.h>

/*************************/
void dec_to_IEEE(){
	/* declare local variables */
	float dec_base_ten = 0;
	int sign = 0;
	int exponent = 0;
	int ex_mask = 1 << 7;
	char sign_str[1];
	char ex_str[9] = "";
	char sig_str[23] = "";
	char IEEE_str[32] = "";
	char sc = '0';

	/* prompt for floating point decimal number */ 
	printf("Enter the decimal representation: ");
	scanf("%f", &dec_base_ten);

	/* Check for 0--if so, print result */
	if(dec_base_ten == 0) {
	   printf("*** Sign: 0 *** Biased Exponent: 00000000 *** Mantissa: 00000000000000000000000 ***The IEEE-754 representation is: 0.000000\n");
		return;
	}

	/* Print sign: if number>0, sign is 0, else 1 */
	/* take absolute value of number before generating significand */
	if(dec_base_ten > 0) {
		sign = 0;
		 sc = '0'; //sign character
	} else {
		sign = 1;
		 sc = '1';
	}
	strcat(sign_str, &sc);
	printf("*** Sign: %d ***", sign);
	
	dec_base_ten = (float)fabs((double)dec_base_ten);
	/* Normalize number:
	while number >2, divide by 2, increment exponent
	while number <1, multiply by 2, decrement exponent
	*/
	while(dec_base_ten >= 2) {
		dec_base_ten = dec_base_ten / 2;
		++exponent;
	}
	while(dec_base_ten < 1) {
		dec_base_ten *= 2;
		--exponent;
	}
    /* Bias exponent by 127 and print each bit in binary with 8-iteration for-loop*/
	exponent += 127;
	printf(" Biased exponent: ");
	
	for(int i = 7; i >= 0; i--) {
		int bit = 0;
		char bchar = '0';
		int ex_result = exponent & ex_mask;
		
		if(ex_result) {
		   bit = 1;
		   bchar = '1';
		   strncat(ex_str, &bchar, 1);
		} else {
		   bchar = '0';
		   strncat(ex_str,  &bchar, 1);
		}
		printf("%d", bit);
		ex_mask = ex_mask >> 1;
	}

	printf(" *** Mantissa: ");
    /* Hide 1 and print significand in binary with 23-iteration for-loop*/
	dec_base_ten -= 1;
	int bit;
	double temp;
	char bitchar;
	for(int j = 0; j < 23; j++) {
	   dec_base_ten *= 2;
	   
	   if(dec_base_ten < 1) {
	      bit = 0;
	      bitchar = '0';
	   } else {
	      bit = 1;
	      bitchar = '1';
	      dec_base_ten = modf(dec_base_ten, &temp);
	   }
	   strncat(sig_str, &bitchar, 1);
	   printf("%d", bit);
	}
	dec_base_ten += 1;
	/* Print IEEE-754 representation */
	char* str; //needs to be here as a pointless filler variable for strtoul
	
	strncat(IEEE_str, &sc, 1);
	strcat(IEEE_str, ex_str);
	strcat(IEEE_str, sig_str);
	
	unsigned long hex = strtoul(IEEE_str, &str, 2);
	
	uint32_t hex_rep = (uint32_t)hex;
	printf(" *** IEEE HEX: %X\n", hex_rep);

  return;
}
/***********************************************************************/
void IEEE_to_dec(){
  /* declare local variables */
  int mask = 1 << 31;
  unsigned int  user_in = 0;
  char in_string[80] = "";
  char ex_bits[8] = "";
  char sign;
  /* prompt for IEEE-754 representation */
  printf("Enter the IEEE-754 representation: "); 
  scanf( "%s", in_string);
  sscanf(in_string, "%X", &user_in);
  
  if(strchr(in_string, '.') != NULL || strlen(in_string) != 8) {
     printf("*** Sign: - ***");
     printf(" Special case: NaN\n");
     return;
  }
	/* check for special cases: 0, -0, +infinity, -infinity, NaN, 
		if so, print and return */ 
		if(user_in == 0x80000000) {
		   printf("*** Sign: - *** Special case: -0\n");
		   return;
		} else if(user_in == 0x00000000) {
		   printf("*** Sign: + *** Special case: +0\n");
		   return;
		} else if(user_in == 0x7FFFFFFF) {
		   printf("*** Sign: + *** Special case: +infinity\n");
		   return;
		} else if(user_in == 0xFF800000) {
		   printf("*** Sign: - *** Special case: -infinity\n");
		   return;
		}

	/* Mask sign from number: if sign=0, print "+", else print "-" */
	int sign_res = (user_in & mask);
	printf("*** Sign: ");
	if(sign_res) {
		printf("-");
		sign = '-';
	} else {
		printf("+");
		sign = '+';
	}
	printf(" ***");
	
	mask = 1 << 30;
	/* Mask biased exponent and significand from number */
	/* If biased exponent=0, number is denormalized with unbiased exponent of -126, 
		print denormalized number as fraction * 2^(-126), return */
		for(int i = 7; i >= 0; i--) {
			char bit_char = '0';
			int ex_res = (user_in & mask);
			if(ex_res) {
				bit_char = '1';
				strncat(ex_bits, &bit_char, 1);
			} else {
			   bit_char = '0';
			   strncat(ex_bits, &bit_char, 1);
			}
			mask = mask >> 1;
		}
		double biased_exponent = 0;
		double ex_counter = 7.0;
		
		for(int j = 0; j  < 8; j++) {
		   if(ex_bits[j] == '1') {
		     biased_exponent += pow(2.0, ex_counter);
		   }
		   ex_counter--;
		}
		int exponent = (int) biased_exponent;
		
	/* Unbias exponent by subtracting 127 and print */
	exponent -= 127;
	
	
	
	/* Add hidden 1 and print normalized decimal number */
	/* Print decimal number */
	mask = 1 << 22;
	
	char sig_bits[23] = "";
	
	for(int k = 22; k >= 0; k--) {
	   char bit_char = '0';
	   int sig_res = (user_in & mask);
	   
	   if(sig_res) {
	      bit_char = '1';
	      strncat(sig_bits, &bit_char, 1);
	   } else {
	      bit_char = '0';
	      strncat(sig_bits, &bit_char, 1);
	   }
	   mask = mask >> 1;
	}
	
	double fraction = 0;
	double sig_counter = -1; //to apply for offset in powers of 2
	
	for(int ii = 0; ii < 23; ii++) {
	   if(sig_bits[ii] == '1') {
	      fraction += pow(2.0, sig_counter);
	   }
	   sig_counter--;
	}
	if(exponent == 128) {
	   printf(" Special case: NaN\n");
	   return;
	}
	fraction += 1;
	
	double pow_two = pow(2.0, (double)exponent);
	printf(" Unbiased Exponent: %d *** ", exponent);
	
	printf("Normalized decimal: %f *** ", fraction);
	
	fraction *= pow_two;
	if(sign == '-') {
	   printf("Decimal: %c%f\n", sign, fraction);
	   return;
	}
	printf("Decimal: %f\n", fraction);
	 return;
	 
}

int main(){
	int choice = 0;
	
	do {
	   printf("Floating-point conversion:\n");
		printf("1) Decimal to IEEE-754 conversion\t2) IEEE-754 to Decimal conversion\t3) Exit\n");
		printf("Enter selection: ");
		scanf("%d", &choice);
		
		switch(choice) {
			case 1:
			printf("\n");
			dec_to_IEEE();
			break;
			case 2:
			IEEE_to_dec();
			break;
		}
	} while(choice != 3);
	printf("*** Program Terminated Normally");
  return 0;
}

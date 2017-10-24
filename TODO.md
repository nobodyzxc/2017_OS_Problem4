Here's some problems needed to be clarified:

1.	Whether a guest(customer) makes a repayment before he meets his need (in (a))?

	Answer: nope.

2.	How many types of thread we need in BP?

   |type 			|number          |functions            |
   |:-------------|:---------------|:--------------------|
   |banker 			|1 				  |serve these customers|
   |customer 		|create by time  |consist transactions |

3.	How many parameters we need in BP?

  - For Poisson Process:

      P = ( lambda^n * exp^(-lambda) ) / n!
      1.	n (	times happend ) : should be a constant, 1.
      2.	lambda ( can be regonized as average times the event happend in a period T ) : variable.
      3.	T ( period ) : should be a constant(e.g. 1 second).

  - For banker:
      1.	Amount of money initialized.

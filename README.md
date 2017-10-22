# 2017_OS_Problem4
repository for problem 4

## Banker's Problem (BP)

A banker has a finite amount of capital, expressed in, say, kronor.
The banker enters into agreements with customers to lend money.
A borrowing customer is a thread.
The following conditions apply:

- (a) The thread is created when the customer specifies a "need",
i.e., a limit that his indebtedness will never be permitted to exceed.

- (b) The thread consists of transactions,
where a transaction is either the advance of a krona by the banker to the customer,
or the repayment of a krona by the customer to the banker.

- (c) The thread ends when the customer repays the last krona to the banker,
and it is understood that this occurs within a finite time after the creation of the thread.

- (d) Requests for an increase in a loan are always granted as long as the current indebtedness
is below the limit established at the creation of the thread,
but the customer may experience a delay between the request and the transfer of the money.

Here a means has to be found for the banker to determine
whether the next payment of a krona to a customer creates the risk of deadlock.

# Operating System Final Project Report

### 題目敘述

### Banker's Problem (BP)
A banker has a finite amount of capital, expressed in, say, kronor. The banker enters into agreements with customers to lend money. A borrowing customer is a thread. The following conditions apply:
- (a) The thread is created when the customer specifies a "need", i.e., a limit that his indebtedness will never be permitted to exceed.
- (b) The thread consists of transactions, where a transaction is either the advance of a krona by the banker to the customer, or the repayment of a krona by the customer to the banker.
- (c\) The thread ends when the customer repays the last krona to the banker, and it is understood that this occurs within a finite time after the creation of the thread.
- (d) Requests for an increase in a loan are always granted as long as the current indebtedness is below the limit established at the creation of the thread, but the customer may experience a delay between the request and the transfer of the money.

Here a means has to be found for the banker to determine whether the next payment of a krona to a customer creates the risk of deadlock.

---
### poisson 公式說明
Poisson process 是基於 Exponential distribution 形成， Exponential distribution 是用以表示獨立隨機事件發生的時間間隔。由於機率密度函數（PDF）是描述隨機變量的輸出值。隨機變量是取值落在某個區域之內的機率則為機率密度函數在這個區域上的積分，亦即累積分布函數（CDF）， CDF 為 

$$P(X,\lambda) = 1 - e^{-\lambda t}$$
 
代表經過時間 t 會有機率 $P(X,\lambda)$ 發生此事件，因此我們將其轉成

$$t = -ln(1-P(X,\lambda))/\lambda$$
以方便求得時間 t ，而 $P(X,\lambda)$ 以亂數生成。

---
### 架構圖

```
C program to simulate the behaviors between bank and clients

                                
  |                          Λ
  | obj info via socket      | slider evnet to adjust lambda
  v                          |
                          
node.js server accepts the information, 
changes its table and sends the update information to web

  |                          Λ
  | obj info via websocket   | slider evnet to adjust lambda
  v                          |                        
  
website updates the information dynamically
                          
                          
```

---
### classes 介紹

為了模擬銀行和客戶之間的互動，所以選擇使用 OOP 來實作。
主要使用了三個 classes 進行互動。

1. Bank (in bank.h)
銀行提供負責處理來自客戶端的請求。
```cpp
class Bank{
    public:
        int count;
        /* count the clients finished */
        Bank(int k ,
                /* initial krona */
                void (*_display)(int , int , int) ,
                /* UI function pointer to display bank status */
                void (*_stopUI)(int));
                /* stop the whole UI or hide the bank status */

        void setLimitPayment(int count);
        /* set total clients generated */
        void active();
        /* start the thread to provide service */
        void close();
        /* stop the service (call  stopUI) */
        void reqKrona(Request *req , int amount);
        /* you can request krona from bank through the function */
        void getPayment(Request* req, int amount);
        /* give the payment to the bank */
        bool reqCheck(pair<Request*, int> newRequest);
        /* check if request can be accepted */
    private:
        int krona;
        /* the current krona bank has */
        int initKrona;
        /* the initial krona bank has */
        int limitPayments;
        /* the number of payments bank should receive */
        void (*display)(int , int , int);
        /* show bank status */
        void (*stopUI)(int);
        /* stop UI or hide bank status */

        pthread_t threadID;
        /* the bank thread */
        pthread_mutex_t queLock;
        /* mutex to protect client's request queue */
        vector<pair<Request* , int> > reqQueue;
        /* client's request queue */
        vector<Request*> personList;
        /* clients who borrow krona from bank */

        static void *running(void *ptr);
        /* thread make it run */
};
```
2. RequestGenerator (in request.h)
負責以 poisson process 產生新的客戶。
```cpp
class RequestGenerator{
    public:
        bool power;
        /* count the ids have distributed */
        RequestGenerator(
                Bank &_bank ,
                void (*_display)(int , int , int));
        /* switch for power on or power off */

        void active(int maximum);
        /* start the thread */
        void genReq(int quo);
        /* generate the clients */
        void flyAway(int idx);
        /* remove child from set */
        
        float cus_lambda;
        /* the lambda of poisson process for generating client */
        float req_lambda;
        /* the lambda of poisson process for generating request */
        void (*display)(int , int , int);
    private:
        Bank &bank;
        pthread_t threadID;
        int maxCust;
        int curCust;
        /* max number of clients it can generate */
        static void *running(void *ptr);
        /* rand a child idx */
        int randIdx();
        pthread_mutex_t baby_taker;
        set<int> childs;
};
```
3. Request (in request.h)
即為客戶物件，會對銀行分段發出請求，直到滿足需求。
```cpp
class Request{
    public:
        bool nextAdvance;
        /* flag to check
         * if the client can start next request */
        int quota , krona , idx;
        /* quota : max krona that client can borrow
         * krona : current krona
         * idx   : client's id */
        
        void (*display)(int , int , int);
        
        /* function to show client's status */
        Request(Bank &bnk ,
                RequestGenerator &gen ,
                int quo , int id);

        void active();
        /* start client's thread */
        void addKrona(int amount);
        /* add client's krona (request succeeded) */
        
    private:
        Bank &bank;
        pthread_t threadID;
        RequestGenerator &generator;

        static void *running(void *ptr);
        /* thread runs the function */
        void advanceKrona(int amount);
        /* send a request to bank */
        void repay();
        /* get all krona , then repay to bank */
};
```

---
### 網頁圖例介紹 (包括顏色變化)
![](https://raw.githubusercontent.com/nobodyzxc/OpSysProj/master/images/demo0.png)

![](https://raw.githubusercontent.com/nobodyzxc/OpSysProj/master/images/demo1.png)

![](https://raw.githubusercontent.com/nobodyzxc/OpSysProj/master/images/demo2.png)

---
### Demo Video

<iframe width="560" height="315" src="https://www.youtube.com/embed/4Plb8SZRdjg" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
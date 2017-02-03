// Based on Dr. Jason Jue's code
// Change part of the logic of simulation in the desciption
// Calculate the theoretical value from the very basic
#include <iostream>
#include "rv.h"
#include "event.h"
#include "math.h"
#include <map>
#include <vector>
using namespace std;

//generate lambda! for coresponding pn

double genLambda(double lambda, int n, int l){
  double ret = 1.0;
  
  //multiply lambda_0 to lambda_n-1
  
  for (int i = 0; i < n; i ++ ){
    ret = ret * (l - i) * lambda;
  }
  return ret;
}

//generate mu! for coresponding pn

double genMu(double mu, int n, int m){
  double ret = 1.0;
  //multiply mu_1 to mu_n
  for(int i = 1; i <= n; i++){
    if(i < m) ret = ret * mu * i;
    else ret = ret * mu * m;
  }
  return ret;

}

//Variables
enum {ARR1, DEP1, ARR2, DEP2};
enum {HIGH, LOW};
vector<Job*> Clist1;
vector<Job*> Clist2;
//Generate Customer
Job* genCustomer(double Pr){
    //Customer* cst = 0;
    double rm = uni_rv();
    if (rm <= Pr) return new Job(HIGH);
    else return new Job(LOW);
    //return cst;
}

//Generate Departure Path 
int GenDepPath(double r2d, double r21, double r22){
    double val = uni_rv();
    if(val <= r2d) return 0;
    else {
        if( val <= r2d + r21) return 1;
        else return 2;
         }
}

//simulate the queueing system m/m/s/k
void simulation(int MaxCount, double lambda, double Pr_High,
        double mu1, double mu2h, double mu2l,
        double r2d, double r21, double r22){
  EventList Elist;                // Create event list
  //enum {ARR1,DEP1,ARR2,DEP2};                 // Define the event types
  //enum {HIGH, LOW}                // Define customer priority
  double clock = 0.0;             // System clock
  int N = 0;                      // Number of customers in system
  int Ndep = 0;                   // Number of departures from system
  double EN = 0.0;                // For calculating E[N]
  int NArrHappen = 0;             // Number of customers entring the system
  double utilization = 0.0;       // For utilization
  int done = 0;                   // End condition satisfied?
  double tao = 0.0;               // For everage time
  int Nblock = 0;                 // Number of blocking customers
  Event* CurrentEvent;            // Point to current event in the event list
  map<int, double> EHash_map;     // For corresponding event arrival and departure time
    bool clist1_busy=false;
    bool clist2_busy=false;
    double para;
    int HDepCL1 = 0;
    int LDepCL1 = 0;
    int HDepCL2 = 0;
    int LDepCL2 = 0;
  //Init the Elist
  Job* initC = genCustomer(Pr_High);
  Elist.insert(clock+exp_rv(lambda), ARR1, initC, true);
  // Start simulation
  int ind = 0;
  while (Ndep < MaxCount)
  {
      
    CurrentEvent = Elist.get();               // Get next Event from list
    double prev = clock;                      // Store old clock value
    clock=CurrentEvent->time;                 // Update system clock
    switch (CurrentEvent->type) {
    case ARR1:                                 // If arrival
        if(CurrentEvent->lbd_source){
            Job* tmp_cst = genCustomer(Pr_High);
            Elist.insert(clock+exp_rv(lambda),ARR1, tmp_cst, true);
      }
        if(!clist1_busy && Clist1.empty()){
        
            clist1_busy = true;
            Elist.insert(clock+exp_rv(mu1),DEP1, CurrentEvent->cst, false);
        }
        else{
            //customerInsert(Clist1, CurrentEvent->cst);
            if(CurrentEvent->cst->priority == LOW) Clist1.push_back(CurrentEvent->cst);
           else{
              int location = 0;
        for( int i = 0; i < Clist1.size(); i++){
            if ( Clist1[i]->priority == LOW) break;
            location ++;

        }
        Clist1.insert(Clist1.begin()+location, CurrentEvent->cst); 
        }
      }
        break;
    case DEP1:                                 // If departure
        clist1_busy = false;
      Elist.insert(clock,ARR2,CurrentEvent->cst, false); // Generate a new arrival event for the corresponding terminal
      if (!Clist1.empty()) {                            // If customer waiting in the queue
          clist1_busy = true;
          Job* tmp = Clist1.front();
          Clist1.erase(Clist1.begin());
        Elist.insert(clock+exp_rv(mu1),DEP1,tmp,false);   //  generate next departure
      }
      if ( CurrentEvent->cst->priority == HIGH) HDepCL1 ++;
      else LDepCL1 ++;
      break;
    case ARR2:
      if(!clist2_busy && Clist2.empty()){
        clist2_busy = true;
       
        if (CurrentEvent->cst->priority == HIGH) para = mu2h;
        else para = mu2l;
        Elist.insert(clock+exp_rv(para), DEP2, CurrentEvent->cst, false);
      }else{
        //customerInsert(Clist2, CurrentEvent-> cst);
        if(CurrentEvent->cst->priority == LOW) Clist2.push_back(CurrentEvent->cst);
           else{
              int location = 0;
        for( int i = 0; i < Clist2.size(); i++){
            if ( Clist2[i]->priority == LOW) break;
            location ++;

        }
        Clist2.insert(Clist2.begin()+location, CurrentEvent->cst); 
        }
      }
      break;
   case DEP2:
      ind ++;
      clist2_busy = false;
      switch (GenDepPath(r2d,r21,r22)){
          case 0:
              Ndep ++;
              break;
          case 1:
              Elist.insert(clock, ARR1, CurrentEvent->cst, false);
              break;
          case 2:
              Elist.insert(clock, ARR2, CurrentEvent->cst, false);
              break;

      }
      if(!Clist2.empty()){
        clist2_busy = true;
        Job* cur_customer = Clist2.front();
        Clist2.erase(Clist2.begin());
        if (cur_customer->priority == HIGH) para = mu2h;
        else para = mu2l;
        Elist.insert(clock + exp_rv(para), DEP2, cur_customer, false);
      }

      if ( CurrentEvent->cst->priority == HIGH) HDepCL2 ++;
      else LDepCL2 ++;

      break;
    }
    delete CurrentEvent;
     
  }

  //cout << "Simu_EN=\t " << EN/clock << endl;
  //cout << "Simu_Tao=\t" << tao/Ndep <<endl;
  //cout << "Simu_Block=\t" << Nblock*1.0/(NArrHappen+Nblock)<<endl;
  //cout << "Simu_Util=\t " << utilization/clock << endl;
  cout << "Clock:\t"<< clock << endl;
  cout << "H1:\t" << HDepCL1 * 1.0 / clock << endl;
  cout << "L1:\t" << LDepCL1 * 1.0 / clock << endl;
  cout << "H2:\t" << HDepCL2 * 1.0 / clock << endl;
  cout << "L2:\t" << LDepCL2 * 1.0 / clock << endl;
  cout << "How much:\t" << ind << endl;
  
  return;
}

int main()
{
  //double rou_set[] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0}; //rou set
  //double mu; //mu
  //double lambda; //lambda
  //int l,m,k; // waiting for input
  ////double lambda = mu * rou * m;
  //cout << "Please input the value of L, m, K, mu"<< endl;
  //cin >> l >> m >> k >> mu;
  //cout <<"==================================================" <<endl;
  //cout <<"=====================Result=======================" <<endl;
  //cout <<"==================================================" <<endl;
  //for(int i = 0; i < 10; i++){
    //lambda = rou_set[i] * mu * m;
    //cout << "lambda = \t" << lambda << endl;
    //simulation(lambda,mu,m,l,min(k,l));
    //calTheoryValue(lambda, mu, m, min(l,k), l);
    //cout <<"==================================================" <<endl;
  //}
  simulation(100000,1.0,0.4,20,10,50,0.25,0.25,0.5);
  return 0;
  
}


#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include<math.h>
#define max 1000
struct task {
	int id,s,m,d,in,out; 
	float p,q;
	/* s:size [MI], m:memory [MB] d:deadline [ms], p:penalty, q:QoS
	in:input file size [KB], out:output file size [KB] */
	int resp; /* resp:response time [ms] */
};
struct node {
	int id,c,m,b,a,d; /* a:available time [ms] d:delay [ms]*/
	float pe,ce,pc,pmin,pmax,ms,eng,procCost,fit; 
	/* pc:processing cost [G$ps] pe: power efficiency, ce: cost efficiency,
	ms: makespan, fit: fitness value */
};
struct task task[max];
struct node fog[max], cloud[max];
int i,j,k,x,y,index,etime,ntask,nfog,ncloud,r,nrun=20; /* etime:execution time, ntask:number of tasks, 
nfog:number of fog nodes, ncloud:number of cloud servers */
float z;
int index_f_max_pe, index_f_max_ce, index_c_max_pe, index_c_max_ce;
float temp, tempq, violationCost=0, PDST=0;
int max_pe, max_ce; // max_pe: max. power eff., max_ce: max. cost eff.
void tasks(int);
void nodes(int, int);
void Random(struct task [],struct node [],struct node [],int,int,int);
void P2C(struct task [],struct node [],struct node [],int,int,int);
void GA(struct task [],struct node [],struct node [],int,int,int);
void Proposed(struct task [],struct node [],struct node [],int,int,int);
void SortTasks(struct task [],int);
main()
{
	int code;
	while(1) {
	printf("\nEnter # of Tasks:\n");
	scanf("%d", &ntask);
//	printf("\nEnter # of fog nodes and # of cloud servers: \n");
//	scanf("%d %d", &nfog, &ncloud);
	nfog=20;
	ncloud=5;
	while (1) {
	printf("_________________________\n");
		printf("\n1-Random");
		printf("\n2-P2C");
		printf("\n3-GA");
		printf("\n4-Proposed");
		printf("\n5-Start New Iteration"); 
		printf("\n6-Exit");
		printf("\n\nEnter Your Choice: ");
		scanf("%d", &code);
		switch(code) {
			case 1: Random(task,fog,cloud,ntask,nfog,ncloud);
					break;
			case 2: P2C(task,fog,cloud,ntask,nfog,ncloud);
					break;
			case 3: GA(task,fog,cloud,ntask,nfog,ncloud);
					break;
			case 4: Proposed(task,fog,cloud,ntask,nfog,ncloud);
					break;
			case 5: break; 
			case 6: exit(1);	
		}
		if (code == 5)
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void tasks(int ntask) 
{
	srand(time(NULL));
//	printf("\n************** TASK INFO. *********************\n");
//	printf("ID\t  Size RAM  Deadline Penalty  QoS  Input Output\n");
	for (i=0; i<ntask; i++) {
		x = rand()%1;
		if (x==0) {
			task[i].s = rand()%9901 + 100; // [100,10000] MI
			task[i].d = rand()%401 + 100; // [100,500] ms
//			printf("%d %d\n",task[i].s, task[i].d);
//			getch();
		}	
		else if (x==1) {
			task[i].s = rand()%3253 + 1028; // [1028,4280] MI
			task[i].d = rand()%2001 + 500; // [500,2500] ms
		}
		else if (x==2) {
			task[i].s = rand()%4662 + 5123; // [5123,9784] MI
			task[i].d = rand()%7501 + 2500; // [2500,10000] ms
		}
			
			
//		task[i].s = rand()%9901 + 100; // [100,10000] MI
		task[i].m = rand()%151 + 50; // [50,200] MB
//		task[i].d = rand()%9901 + 100; // [100,10000] ms
		task[i].p =(rand()%50+1)/100.0;
		task[i].q =(rand()%1000+9000)/100.0;
		task[i].in = rand()%9901 + 100; // [100KB,10MB]
		task[i].out = rand()%991 + 1; // [1KB,1MB]
		task[i].id = i;
//		printf("task[%d] = %-4d %-4d %-8d %-8.2f %-5.2f %-5d %-5d\n", task[i].id, task[i].s, task[i].m,
//		task[i].d, task[i].p, task[i].q, task[i].in, task[i].out);
	}
//	printf("\n");
}
void nodes(int nfog, int ncloud)
{
//	printf("\n************** Nodes INFO. *********************\n");
//	printf("ID\t CPU  RAM  BW   PC   Delay  P_Max P_Min\n");
	for (i=0; i<nfog; i++) {
		fog[i].c = rand()%1001 + 500; // [500,1500] MIPS
		fog[i].m = rand()%101 + 150; // [512,8192] MB
		fog[i].b = rand()%991 + 10; // [10,1000] Mbps
		fog[i].pc = (rand()%31 + 10)/100.0; // [0.1,0.4] G$ps
		fog[i].d = rand()%10 + 1; // [1,10] ms
		fog[i].pmax = rand()%61 + 40; // [40,100] W
		fog[i].pmin = (rand()%1 + 60)/100.0*fog[i].pmax;
		fog[i].id = i;
//		printf("fog[%d] = %-4d %-4d %-4d %-5.2f %-6d %-5g %-5g\n", fog[i].id, fog[i].c, fog[i].m, fog[i].b,
//		fog[i].pc, fog[i].d, fog[i].pmax, fog[i].pmin);
	}
//	printf("\nID\t   CPU   RAM   BW   PC    Delay  P_Max P_Min\n");
	x = rand()%301 + 200; // [200,500] ms
	for (i=0; i<ncloud; i++) {
		cloud[i].c = rand()%2001 + 3000; // [3000,5000] MIPS
		cloud[i].m = rand()%57345 + 8192; // [8192,65536] MB
		cloud[i].b = rand()%9901 + 100; // [100,10000] Mbps
		cloud[i].pc = (rand()%31 + 70)/100.0; // [0.7,1] G$ps
		cloud[i].d = x;
		cloud[i].pmax = rand()%201 + 200; // [200,400] W
		cloud[i].pmin = (rand()%1 + 60)/100.0*cloud[i].pmax;
		cloud[i].id = i;
//		printf("cloud[%d] = %-5d %-5d %-4d %-5.2f %-6d %-5g %-5g\n", cloud[i].id, cloud[i].c, cloud[i].m, cloud[i].b,
//		cloud[i].pc, cloud[i].d, cloud[i].pmax, cloud[i].pmin);
	}
//	printf("\n");
}

/////////////////////////  SortTasks  ////////////////////////////////
void SortTasks(struct task T[], int)  // based on their deadline in increasing order
{
	struct task temp;
	for (i=0; i<ntask-1; i++)
		for (j=i+1; j<ntask; j++)
			if (T[j].d < T[i].d){
				temp = T[j];
				T[j] = T[i];
				T[i] = temp;	
			}
}

/////////////////////////  Random  ////////////////////////////////
void Random(struct task task[],struct node fog[],struct node cloud[],int ntask,int nfog,int ncloud)
{	
	float sumM=0, sumengCons=0, sumprocCost=0, sumfitValue=0;
	for (r=1; r<=nrun; r++) {
//	printf("\nRun=%d\n",r);
	struct task T[max];
	struct node F[max], C[max];
	tasks(ntask);
	nodes(nfog, ncloud);
	int M=0, totalPenalty=0, minE; // M:Makespan, minE:Minimum_Executiontime
	int totTaskSize=0, totNodeCPU=0; //totTaskSize: size of all tasks totNodeCPU: CPU power of all nodes
	float f_max_pe=0, f_max_ce=0, c_max_pe=0, c_max_ce=0;
	float min_engCons=0, min_procCost=0, min_M=0; //min_M: min. makespan
	float procCost=0, engCons=0, fitValue=0;
	float a=0.34, b=0.33, c=0.33; //coefficients a: eng, b: cost, c: makespan
	for (i=0; i<ntask; i++) {
		T[i] = task[i];
		T[i].resp = 0;
	}
	for (i=0; i<nfog; i++) {
		F[i] = fog[i];
		F[i].a = 0;
		F[i].procCost = 0;
		F[i].eng = 0;
	}
	for (i=0; i<ncloud; i++) {
		C[i] = cloud[i];
		C[i].a = 0;
		C[i].procCost = 0;
		C[i].eng = 0;
	} 	
	
	//calculating the power and cost efficiency of nodes
	for (i=0; i<nfog; i++) {
		F[i].pe = F[i].c/F[i].pmax;
		F[i].ce = F[i].c/F[i].pc;
		if (F[i].pe > f_max_pe) {
			f_max_pe = F[i].pe;
			index_f_max_pe = i;
		}
		if (F[i].ce > f_max_ce) {
			f_max_ce = F[i].ce;
			index_f_max_ce = i;
		}
//		printf("\n*************************************\n");
//		printf("F[%d].pe=%-8.2f, F[%d].ce=%-8.2f\n",F[i].id,F[i].pe,F[i].id,F[i].ce);
//		getch();
	}
//	printf("\nf_max_pe=%-8.2f  ,  f_max_ce=%-8.2f\n\n",f_max_pe, f_max_ce);
//	getch();
	for (i=0; i<ncloud; i++) {
		C[i].pe = C[i].c/C[i].pmax;
		C[i].ce = C[i].c/C[i].pc;
		if (C[i].pe > c_max_pe) {
			c_max_pe = C[i].pe;
			index_c_max_pe = i;
		}	
		if (C[i].ce > c_max_ce) {
			c_max_ce = C[i].ce;
			index_c_max_ce = i;
		}	
//		printf("C[%d].pe=%-8.2f, C[%d].ce=%-8.2f\n",C[i].id,C[i].pe,C[i].id,C[i].ce);
	}

	for (i=0; i<ntask; i++) {
//		printf("\nT[%d] is ready to be scheduled!\n",T[i].id);
		x = rand()%(nfog+ncloud);
//		printf("x=%d\n",x);
		if (x<nfog) {
			etime = (float)T[i].s / F[x].c * 1000;
			F[x].a += etime; 
			T[i].resp = F[x].a + F[x].d;
			F[x].procCost += ((float)T[i].s/F[x].c*F[x].pc);
//			printf("F[%d].a=%d\n",F[x].id, F[x].a);
//			getch();
		}
		else {
			x = x%ncloud;
			etime = (float)T[i].s/C[x].c*1000;
			C[x].a += etime;
			T[i].resp = C[x].a + C[x].d;
			C[x].procCost += ((float)T[i].s/C[x].c*C[x].pc);
//			printf("C[%d].a=%d\n",C[x].id, C[x].a);
//			getch();
		}
	}
//	printf("\n----------Results----------\n");
	violationCost = 0;
	PDST=0;
	for(i=0; i<ntask; i++) {
//		printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
		temp = T[i].resp - T[i].d;
		if (temp < 0)
			temp = 0;
//		printf("temp = %g\n",temp);
		tempq = temp*100/T[i].d - 100 + T[i].q;
		if (tempq>0)
			violationCost += (tempq * T[i].p);
//		printf("total violationCost: %g\n\n",violationCost);
//		getch();	
		if (T[i].resp > T[i].d) 
			totalPenalty += T[i].resp - T[i].d;
		else
			PDST++;	
	}
	// calculating makespan and degree of imbalance (DI)
	int sumExe=0, avgExe, minExe = 10000;
	float DI; // in ms
	for(i=0; i<nfog; i++) {
		if(F[i].a > M)
			M = F[i].a;
		if(F[i].a < minExe)
			minExe = F[i].a;
		sumExe += F[i].a;
//		printf("F[%d].a=%d\n",i,F[i].a);
	}

	for(i=0; i<ncloud; i++) {
		if(C[i].a > M)
			M = C[i].a;
		if(C[i].a < minExe)
			minExe = C[i].a;
		sumExe += C[i].a;
//		printf("C[%d].a=%d\n",i,C[i].a);
	}
	avgExe = sumExe / (nfog+ncloud);
	DI = 1.0*(M-minExe) / avgExe;
	
	for(i=0; i<nfog; i++) {
		F[i].eng = F[i].a/1000.0*F[i].pmax + (M/1000.0-F[i].a/1000.0)*F[i].pmin; //ws or j
		engCons += 	F[i].eng;
		procCost += F[i].procCost;	
	}
	for(i=0; i<ncloud; i++) {
		C[i].eng = C[i].a/1000.0*C[i].pmax + (M/1000.0-C[i].a/1000.0)*C[i].pmin; //ws or j
		engCons += 	C[i].eng;
		procCost += C[i].procCost;	
	}
	//calculating min_engCons, min_procCost, min_M
	for (i=0; i<ntask; i++)
		totTaskSize += T[i].s;
	for (i=0; i<nfog; i++)
		totNodeCPU += F[i].c;
	for (i=0; i<ncloud; i++)
		totNodeCPU += C[i].c;
	min_M = 1.0*totTaskSize / totNodeCPU; // in second
	if (f_max_pe > c_max_pe) {
		min_engCons = 1.0*totTaskSize/F[index_f_max_pe].c*F[index_f_max_pe].pmax;
//		printf("F[%d] is the most power eff. node\n",F[index_f_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*F[index_f_max_pe].pmax;
	}
	else {
		min_engCons = 1.0*totTaskSize/C[index_c_max_pe].c*C[index_c_max_pe].pmax;
//		printf("C[%d] is the most power eff. node\n",C[index_c_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*C[index_c_max_pe].pmax;
	}		
	if (f_max_ce > c_max_ce) {
//		printf("totTaskSize=%d , index=%d , MIPS=%d , procCost=%0.2f\n",totTaskSize,index_f_max_ce,F[index_f_max_ce].c,F[index_f_max_ce].pc);
		min_procCost = 1.0*totTaskSize/F[index_f_max_ce].c*F[index_f_max_ce].pc;
//		printf("F[%d] is the most cost eff. node\n",F[index_f_max_ce].id);
	}	
	else {
		min_procCost = 1.0*totTaskSize/C[index_c_max_ce].c*C[index_c_max_ce].pc;
//		printf("C[%d] is the most cost eff. node\n",C[index_c_max_ce].id);
	}
	
//	printf("totTaskSize=%d\n",totTaskSize);
//	printf("totNodeCPU=%d\n",totNodeCPU);
//	printf("min_engCons=%.2f , min_procCost=%.2f , min_M=%.2f\n",min_engCons,min_procCost,min_M);
	fitValue = a*min_engCons/engCons + b*min_procCost/procCost + c*min_M/M*1000;
//	printf("a*min_engCons/engCons=%f\n",a*min_engCons/engCons);
//	printf("b*min_procCost/procCost=%f\n",b*min_procCost/procCost);
//	printf("c*min_M/M=%f\n",c*min_M/M*1000);
//	printf("\nengCons: %.2f",engCons);
//	printf("\nprocCost: %.2f",procCost);
//	printf("\nMakespan: %.2f",M/1000.0);
//	printf("\nfitValue: %.2f",fitValue);
//	printf("\nDI: %.2f",DI);
//	printf("\ntotalPenalty: %d",totalPenalty/1000);
//	printf("\ntotal violationCost: %g",violationCost);
//	printf("\nPDST: %g\n\n",PDST/ntask);
//	getch();
	sumM += M;
	sumengCons += engCons;
	sumprocCost += procCost;
	sumfitValue += fitValue;
	}
	printf("\n*************Final Reults*************\n");
	printf("\nMakespan: %.2f",sumM/1000.0/nrun);
	printf("\nengCons: %.2f",sumengCons/nrun);
	printf("\nprocCost: %.2f",sumprocCost/nrun);
	printf("\nfitValue: %.2f\n",sumfitValue/nrun);
}

/////////////////////////  Power of Two Choices  ////////////////////////////////
void P2C(struct task task[],struct node fog[],struct node cloud[],int ntask,int nfog,int ncloud)
{	
	float sumM=0, sumengCons=0, sumprocCost=0, sumfitValue=0;
	for (r=1; r<=nrun; r++) {
//	printf("\nRun=%d\n",r);
	struct task T[max];
	struct node F[max], C[max];
	tasks(ntask);
	nodes(nfog, ncloud);
	int M=0, totalPenalty=0, minE; // M:Makespan, minE:Minimum_Executiontime
	int fxflag, cxflag, fyflag, cyflag;
	int totTaskSize=0, totNodeCPU=0; //totTaskSize: size of all tasks totNodeCPU: CPU power of all nodes
	float f_max_pe=0, f_max_ce=0, c_max_pe=0, c_max_ce=0;
	float min_engCons=0, min_procCost=0, min_M=0; //min_M: min. makespan
	float procCost=0, engCons=0, fitValue=0;
	float a=0.34, b=0.33, c=0.33; //coefficients a: eng, b: cost, c: makespan
	for (i=0; i<ntask; i++) {
		T[i] = task[i];
		T[i].resp = 0;
	}
	for (i=0; i<nfog; i++) {
		F[i] = fog[i];
		F[i].a = 0;
		F[i].procCost = 0;
		F[i].eng = 0;
	}
	for (i=0; i<ncloud; i++) {
		C[i] = cloud[i];
		C[i].a = 0;
		C[i].procCost = 0;
		C[i].eng = 0;
	} 	
	
		//calculating the power and cost efficiency of nodes
	for (i=0; i<nfog; i++) {
		F[i].pe = F[i].c/F[i].pmax;
		F[i].ce = F[i].c/F[i].pc;
		if (F[i].pe > f_max_pe) {
			f_max_pe = F[i].pe;
			index_f_max_pe = i;
		}
		if (F[i].ce > f_max_ce) {
			f_max_ce = F[i].ce;
			index_f_max_ce = i;
		}
//		printf("F[%d].pe=%-8.2f, F[%d].ce=%-8.2f\n",F[i].id,F[i].pe,F[i].id,F[i].ce);
	}
//	printf("\nf_max_pe=%-8.2f  ,  f_max_ce=%-8.2f\n\n",f_max_pe, f_max_ce);
	for (i=0; i<ncloud; i++) {
		C[i].pe = C[i].c/C[i].pmax;
		C[i].ce = C[i].c/C[i].pc;
		if (C[i].pe > c_max_pe) {
			c_max_pe = C[i].pe;
			index_c_max_pe = i;
		}	
		if (C[i].ce > c_max_ce) {
			c_max_ce = C[i].ce;
			index_c_max_ce = i;
		}	
//		printf("C[%d].pe=%-8.2f, C[%d].ce=%-8.2f\n",C[i].id,C[i].pe,C[i].id,C[i].ce);
	}

	for (i=0; i<ntask; i++) {
//		printf("\nT[%d] is ready to be scheduled!\n",T[i].id);
		fxflag=cxflag=fyflag=cyflag=0;
		x = rand()%(nfog+ncloud);
		y = rand()%(nfog+ncloud);
		if (x<nfog) {
			fxflag=1;
			etime = (float)T[i].s / F[x].c * 1000;
			F[x].ms = F[x].a + etime; 
//			printf("F[%d].ms=%.2f\n",F[x].id, F[x].ms);
		}
		else {
			cxflag=1;
			x = x%ncloud;
			etime = (float)T[i].s / C[x].c * 1000;
			C[x].ms = C[x].a + etime; 
//			printf("C[%d].ms=%.2f\n",C[x].id, C[x].ms);
		}
		if (y<nfog) {
			fyflag=1;
			etime = (float)T[i].s / F[y].c * 1000;
			F[y].ms = F[y].a + etime; 
//			printf("F[%d].ms=%.2f\n",F[y].id, F[y].ms);
		}
		else {
			cyflag=1;
			y = y%ncloud;
			etime = (float)T[i].s / C[y].c * 1000;
			C[y].ms = C[y].a + etime; 
//			printf("C[%d].ms=%.2f\n",C[y].id, C[y].ms);
		}
		if(fxflag==1 and fyflag==1)
			if (F[x].ms<=F[y].ms) {
				F[x].a += (float)T[i].s/F[x].c*1000;
				T[i].resp = F[x].a + F[x].d;
				F[x].procCost += ((float)T[i].s/F[x].c*F[x].pc);
//				printf("T[%d] --> F[%d]\n", T[i].id, F[x].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("F[%d].a is: %d\n\n",F[x].id,F[x].a);
//				getch();
			}
			else {
				F[y].a += (float)T[i].s/F[y].c*1000;
				T[i].resp = F[y].a + F[y].d;
				F[y].procCost += ((float)T[i].s/F[y].c*F[y].pc);
//				printf("T[%d] --> F[%d]\n", T[i].id, F[y].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("F[%d].a is: %d\n\n",F[y].id,F[y].a);
//				getch();
			}
		else if(fxflag==1 and cyflag==1)
			if (F[x].ms<=C[y].ms) {
				F[x].a += (float)T[i].s/F[x].c*1000;
				T[i].resp = F[x].a + F[x].d;
				F[x].procCost += ((float)T[i].s/F[x].c*F[x].pc);
//				printf("T[%d] --> F[%d]\n", T[i].id, F[x].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("F[%d].a is: %d\n\n",F[x].id,F[x].a);
//				getch();
			}
			else {
				C[y].a += (float)T[i].s/C[y].c*1000;
				T[i].resp = C[y].a + C[y].d;
				C[y].procCost += ((float)T[i].s/C[y].c*C[y].pc);
//				printf("T[%d] --> C[%d]\n", T[i].id, C[y].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("C[%d].a is: %d\n\n",C[y].id,C[y].a);
//				getch();
			}
		else if(cxflag==1 and fyflag==1)
			if (C[x].ms<=F[y].ms) {
				C[x].a += (float)T[i].s/C[x].c*1000;
				T[i].resp = C[x].a + C[x].d;
				C[x].procCost += ((float)T[i].s/C[x].c*C[x].pc);
//				printf("T[%d] --> C[%d]\n", T[i].id, C[x].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("C[%d].a is: %d\n\n",C[x].id,C[x].a);
//				getch();
			}
			else {
				F[y].a += (float)T[i].s/F[y].c*1000;
				T[i].resp = F[y].a + F[y].d;
				F[y].procCost += ((float)T[i].s/F[y].c*F[y].pc);
//				printf("T[%d] --> F[%d]\n", T[i].id, F[y].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("F[%d].a is: %d\n\n",F[y].id,F[y].a);
//				getch();
			}
		else if(cxflag==1 and cyflag==1)
			if (C[x].ms<=C[y].ms) {
				C[x].a += (float)T[i].s/C[x].c*1000;
				T[i].resp = C[x].a + C[x].d;
				C[x].procCost += ((float)T[i].s/C[x].c*C[x].pc);
//				printf("T[%d] --> C[%d]\n", T[i].id, C[x].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("C[%d].a is: %d\n\n",C[x].id,C[x].a);
//				getch();
			}
			else {
				C[y].a += (float)T[i].s/C[y].c*1000;
				T[i].resp = C[y].a + F[y].d;
				C[y].procCost += ((float)T[i].s/C[y].c*C[y].pc);
//				printf("T[%d] --> C[%d]\n", T[i].id, C[y].id);
//				printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//				printf("C[%d].a is: %d\n\n",C[y].id,C[y].a);
//				getch();
			}
	}
//	printf("\n----------Results----------\n");
	violationCost = 0;
	PDST=0;
	for(i=0; i<ntask; i++) {
//		printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
		temp = T[i].resp - T[i].d;
		if (temp < 0)
			temp = 0;
//		printf("temp = %g\n",temp);
		tempq = temp*100/T[i].d - 100 + T[i].q;
		if (tempq>0)
			violationCost += (tempq * T[i].p);
//		printf("total violationCost: %g\n\n",violationCost);
//		getch();	
		if (T[i].resp > T[i].d) 
			totalPenalty += T[i].resp - T[i].d;
		else
			PDST++;	
	}
	// calculating makespan and degree of imbalance (DI)
	int sumExe=0, avgExe, minExe = 10000;
	float DI; // in ms
	for(i=0; i<nfog; i++) {
		if(F[i].a > M)
			M = F[i].a;
		if(F[i].a < minExe)
			minExe = F[i].a;
		sumExe += F[i].a;
//		printf("F[%d].a=%d\n",i,F[i].a);
	}

	for(i=0; i<ncloud; i++) {
		if(C[i].a > M)
			M = C[i].a;
		if(C[i].a < minExe)
			minExe = C[i].a;
		sumExe += C[i].a;
//		printf("C[%d].a=%d\n",i,C[i].a);
	}
	avgExe = sumExe / (nfog+ncloud);
	DI = 1.0*(M-minExe) / avgExe;
	for(i=0; i<nfog; i++) {
		F[i].eng = F[i].a/1000.0*F[i].pmax + (M/1000.0-F[i].a/1000.0)*F[i].pmin; //ws or mj
		engCons += 	F[i].eng;
		procCost += F[i].procCost;	
	}
	for(i=0; i<ncloud; i++) {
		C[i].eng = C[i].a/1000.0*C[i].pmax + (M/1000.0-C[i].a/1000.0)*C[i].pmin; //ws or mj
		engCons += 	C[i].eng;
		procCost += C[i].procCost;	
	}
	//calculating min_engCons, min_procCost, min_M
	for (i=0; i<ntask; i++)
		totTaskSize += T[i].s;
	for (i=0; i<nfog; i++)
		totNodeCPU += F[i].c;
	for (i=0; i<ncloud; i++)
		totNodeCPU += C[i].c;
	min_M = 1.0*totTaskSize / totNodeCPU; // in second
	if (f_max_pe > c_max_pe) {
		min_engCons = 1.0*totTaskSize/F[index_f_max_pe].c*F[index_f_max_pe].pmax;
//		printf("F[%d] is the most power eff. node\n",F[index_f_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*F[index_f_max_pe].pmax;

	}
	else {
		min_engCons = 1.0*totTaskSize/C[index_c_max_pe].c*C[index_c_max_pe].pmax;
//		printf("C[%d] is the most power eff. node\n",C[index_c_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*C[index_c_max_pe].pmax;

	}		
	if (f_max_ce > c_max_ce) {
//		printf("totTaskSize=%d , index=%d , MIPS=%d , procCost=%0.2f\n",totTaskSize,index_f_max_ce,F[index_f_max_ce].c,F[index_f_max_ce].pc);
		min_procCost = 1.0*totTaskSize/F[index_f_max_ce].c*F[index_f_max_ce].pc;
//		printf("F[%d] is the most cost eff. node\n",F[index_f_max_ce].id);
	}	
	else {
		min_procCost = 1.0*totTaskSize/C[index_c_max_ce].c*C[index_c_max_ce].pc;
//		printf("C[%d] is the most cost eff. node\n",C[index_c_max_ce].id);
	}
		
//	printf("totTaskSize=%d\n",totTaskSize);
//	printf("totNodeCPU=%d\n",totNodeCPU);
//	printf("min_engCons=%.2f , min_procCost=%.2f , min_M=%.2f\n",min_engCons,min_procCost,min_M);
	fitValue = a*min_engCons/engCons + b*min_procCost/procCost + c*min_M/M*1000;
//	printf("a*min_engCons/engCons=%f\n",a*min_engCons/engCons);
//	printf("b*min_procCost/procCost=%f\n",b*min_procCost/procCost);
//	printf("c*min_M/M=%f\n",c*min_M/M*1000);
//	printf("\nengCons: %.2f",engCons);
//	printf("\nprocCost: %.2f",procCost);
//	printf("\nMakespan: %.2f",M/1000.0);
//	printf("\nfitValue: %.2f",fitValue);
//	printf("\nDI: %.2f",DI);
//	printf("\ntotalPenalty: %d",totalPenalty/1000);
//	printf("\ntotal violationCost: %g",violationCost);
//	printf("\nPDST: %g\n",PDST/ntask);
//	getch();
	sumM += M;
	sumengCons += engCons;
	sumprocCost += procCost;
	sumfitValue += fitValue;
	}
	printf("\n*************Final Reults*************\n");
	printf("\nMakespan: %.2f",sumM/1000.0/nrun);
	printf("\nengCons: %.2f",sumengCons/nrun);
	printf("\nprocCost: %.2f",sumprocCost/nrun);
	printf("\nfitValue: %.2f\n",sumfitValue/nrun);
}

/////////////////////////  GA  ////////////////////////////////
void GA(struct task task[],struct node fog[],struct node cloud[],int ntask,int nfog,int ncloud)
{	
	float sumM=0, sumengCons=0, sumprocCost=0, sumfitValue=0, mrate=0.1,
	 solM[200], solC[200], solOF[200], sumF=0; //mrate:mutationRate
	int npop=100, sol[200][1000], itr, Nitr=500; //npopo:initialPopulation#, sol:solutions
	for (r=1; r<=nrun; r++) {
//	printf("\nRun=%d\n",r);
	struct task T[max];
	struct node F[max], C[max];
	tasks(ntask);
	nodes(nfog, ncloud);
	int M=0, totalPenalty=0, minE; // M:Makespan, minE:Minimum_Executiontime
	int totTaskSize=0, totNodeCPU=0; //totTaskSize: size of all tasks totNodeCPU: CPU power of all nodes
	float f_max_pe=0, f_max_ce=0, c_max_pe=0, c_max_ce=0;
	float min_engCons=0, min_procCost=0, min_M=0, maxOF=0; //min_M: min. makespan
	float procCost=0, engCons=0, fitValue=0;
	float a=0.34, b=0.33, c=0.33; //coefficients a: eng, b: cost, c: makespan
	for (i=0; i<ntask; i++) {
		T[i] = task[i];
		T[i].resp = 0;
	}
	for (i=0; i<nfog; i++) {
		F[i] = fog[i];
		F[i].a = 0;
		F[i].procCost = 0;
		F[i].eng = 0;
	}
	for (i=0; i<ncloud; i++) {
		C[i] = cloud[i];
		C[i].a = 0;
		C[i].procCost = 0;
		C[i].eng = 0;
	} 	
		//calculating min_M and min_procCost
	for (i=0; i<ntask; i++)
		totTaskSize += T[i].s;
	for (i=0; i<nfog; i++)
		totNodeCPU += F[i].c;
	for (i=0; i<ncloud; i++)
		totNodeCPU += C[i].c;
		
		//calculating the power and cost efficiency of nodes
	for (i=0; i<nfog; i++) {
		F[i].pe = F[i].c/F[i].pmax;
		F[i].ce = F[i].c/F[i].pc;
		if (F[i].pe > f_max_pe) {
			f_max_pe = F[i].pe;
			index_f_max_pe = i;
		}
		if (F[i].ce > f_max_ce) {
			f_max_ce = F[i].ce;
			index_f_max_ce = i;
		}
//		printf("F[%d].pe=%-8.2f, F[%d].ce=%-8.2f\n",F[i].id,F[i].pe,F[i].id,F[i].ce);
	}
//	printf("\nf_max_pe=%-8.2f  ,  f_max_ce=%-8.2f\n\n",f_max_pe, f_max_ce);
	for (i=0; i<ncloud; i++) {
		C[i].pe = C[i].c/C[i].pmax;
		C[i].ce = C[i].c/C[i].pc;
		if (C[i].pe > c_max_pe) {
			c_max_pe = C[i].pe;
			index_c_max_pe = i;
		}	
		if (C[i].ce > c_max_ce) {
			c_max_ce = C[i].ce;
			index_c_max_ce = i;
		}	
//		printf("C[%d].pe=%-8.2f, C[%d].ce=%-8.2f\n",C[i].id,C[i].pe,C[i].id,C[i].ce);
	}	
			
	min_M = 1.0*totTaskSize / totNodeCPU; // in second	
	if (f_max_ce > c_max_ce) {
//		printf("totTaskSize=%d , index=%d , MIPS=%d , procCost=%0.2f\n",totTaskSize,index_f_max_ce,F[index_f_max_ce].c,F[index_f_max_ce].pc);
		min_procCost = 1.0*totTaskSize/F[index_f_max_ce].c*F[index_f_max_ce].pc;
//		printf("F[%d] is the most cost eff. node\n",F[index_f_max_ce].id);
	}	
	else {
		min_procCost = 1.0*totTaskSize/C[index_c_max_ce].c*C[index_c_max_ce].pc;
//		printf("C[%d] is the most cost eff. node\n",C[index_c_max_ce].id);
	}
//	printf("min_M=%g\n",min_M);	
//	printf("min_procCost=%g\n",min_procCost);	
	// creating initial population
	for(i=0; i<npop; i++)
		for(j=0; j<ntask; j++) {
			x = rand()%(nfog+ncloud);
			sol[i][j]=x;
		}	
//	printf("Initial population....\n");
//	for(i=0; i<npop; i++) {
//		for(j=0; j<ntask; j++) 
//			printf("%4d",sol[i][j]);
//		printf("\n");
//	}
	// strating the algorithm...
	for (itr=0; itr<Nitr; itr++) {
//		printf("-----------------------------------------------------\n");
//		printf("itr=%d...\n",itr);	
	// calculating the makespan and cost of each solution, i.e., fitness 
	for(i=0; i<npop; i++) {
		for (k=0; k<nfog; k++) {
			F[k].a = 0;
			F[k].procCost = 0;
		}
		for (k=0; k<ncloud; k++) {
			C[k].a = 0;
			C[k].procCost = 0;
		} 
		procCost=0;
		M=0;
		for(j=0; j<ntask; j++) {
//			printf("sol[%d][%d]=%d \n", i,j,sol[i][j]);
			if(sol[i][j]<nfog) {
				etime = (float)T[j].s / F[sol[i][j]].c * 1000;
				F[sol[i][j]].a += etime; 
				F[sol[i][j]].procCost += ((float)T[j].s/F[sol[i][j]].c*F[sol[i][j]].pc);
//				printf("j=%d\n",j);
//				printf("etime(s)=%g\n",(float)T[j].s/F[sol[i][j]].c);
//				printf("procCost=%g\n",((float)T[j].s/F[sol[i][j]].c*F[sol[i][j]].pc));
//				printf("F[%d].procCost=%g\n",sol[i][j],F[sol[i][j]].procCost);
//				getch();
			}
			else {
				x = sol[i][j]%nfog;
				etime = (float)T[j].s/C[x].c*1000;
				C[x].a += etime;
				C[x].procCost += ((float)T[j].s/C[x].c*C[x].pc);
//				printf("j=%d\n",j);
//				printf("etime(s)=%g\n",(float)T[j].s/C[x].c);
//				printf("procCost=%g\n",((float)T[j].s/C[x].c*C[x].pc));
//				printf("C[%d].procCost=%g\n",x,C[x].procCost);
//				getch();
			}	
		}
//		printf("\npop#%d...\n",i);
		for(k=0; k<nfog; k++) {
			procCost += F[k].procCost;
			if(F[k].a > M)
				M = F[k].a;
//			printf("F[%d].a=%d\n",k,F[k].a);
		}
		for(k=0; k<ncloud; k++) {
			procCost += C[k].procCost;
			if(C[k].a > M)
				 M = C[k].a;
//			printf("C[%d].a=%d\n",k,C[k].a);
		}
		solM[i]=1.0*M;
		solC[i]=procCost;
		solOF[i] = 0.67*min_M/solM[i] + 0.33*min_procCost/solC[i];
//		printf("pop#%d:solM=%g, solC=%g, solOF=%g \n", i,solM[i],solC[i],solOF[i]);
//		getch();
	}
	// Selection and Crossover Phase
	for(i=0; i<npop; i++)
		sumF += solOF[i];
//	printf("sumF=%g\n",sumF);
	for(i=0; i<npop; i++)
		solOF[i] = solOF[i]/sumF;
	for(i=1; i<npop; i++)
		solOF[i] += solOF[i-1];
//	for(i=0; i<npop; i++)	
//		printf("solOF[%d]=%g\n",i,solOF[i]);
		
	for(i=0; i<0.5*npop; i++) {
		z = 1.0*rand()/RAND_MAX;
//		printf("z=%g\n",z);
		for(k=0; k<npop; k++)
			if(z <= solOF[k]) {
				x=k;
				break;
			}
//		z = 1.0*rand()/RAND_MAX;
//		for(k=0; k<npop; k++)
//			if(z <= solOF[k]) {
//				y=k;
//				break;
//			}
		y = rand()%npop;
//		printf("x=%d, y=%d\n", x,y);
		for(j=0; j<ntask; j++) {
			if(j<ntask/2) {
				sol[2*i][j] = sol[x][j];
				sol[2*i+1][j] = sol[y][j];
			}
			else {
				sol[2*i][j] = sol[y][j];
				sol[2*i+1][j] = sol[x][j];
			}
		}		
	}
//	for(i=0; i<npop; i++) {
//		for(j=0; j<ntask; j++) 
//			printf("%4d",sol[i][j]);
//		printf("\n");
//	}
	// mutation
	for(i=0; i<npop; i++) {
		z = 1.0*rand()/RAND_MAX;
		if(z<mrate) {
//			printf("mutation accured for pop#%d\n",i);
			j=rand()%ntask;
			sol[i][j]=rand()%ntask;
//			printf("j=%d, sol[i][j]=%d\n",j,sol[i][j]);
		}
	}
//	printf("\nNew population...\n");
//	for(i=0; i<npop; i++) {
//		for(j=0; j<ntask; j++) 
//			printf("%4d",sol[i][j]);
//		printf("\n");
//	}
//	getch();
	}  // end of the alg.
	// calculating the solM, solC and solOF for final population
	for(i=0; i<npop; i++) {
		for (k=0; k<nfog; k++) {
			F[k].a = 0;
			F[k].procCost = 0;
		}
		for (k=0; k<ncloud; k++) {
			C[k].a = 0;
			C[k].procCost = 0;
		} 
		procCost=0;
		M=0;
		for(j=0; j<ntask; j++) {
//			printf("sol[%d][%d]=%d \n", i,j,sol[i][j]);
			if(sol[i][j]<nfog) {
				etime = (float)T[j].s / F[sol[i][j]].c * 1000;
				F[sol[i][j]].a += etime; 
				F[sol[i][j]].procCost += ((float)T[j].s/F[sol[i][j]].c*F[sol[i][j]].pc);
//				printf("j=%d\n",j);
//				printf("etime(s)=%g\n",(float)T[j].s/F[sol[i][j]].c);
//				printf("procCost=%g\n",((float)T[j].s/F[sol[i][j]].c*F[sol[i][j]].pc));
//				printf("F[%d].procCost=%g\n",sol[i][j],F[sol[i][j]].procCost);
//				getch();
			}
			else {
				x = sol[i][j]%nfog;
				etime = (float)T[j].s/C[x].c*1000;
				C[x].a += etime;
				C[x].procCost += ((float)T[j].s/C[x].c*C[x].pc);
//				printf("j=%d\n",j);
//				printf("etime(s)=%g\n",(float)T[j].s/C[x].c);
//				printf("procCost=%g\n",((float)T[j].s/C[x].c*C[x].pc));
//				printf("C[%d].procCost=%g\n",x,C[x].procCost);
//				getch();
			}	
		}
//		printf("\npop#%d...\n",i);
		for(k=0; k<nfog; k++) {
			procCost += F[k].procCost;
			if(F[k].a > M)
				M = F[k].a;
//			printf("F[%d].a=%d\n",k,F[k].a);
		}
		for(k=0; k<ncloud; k++) {
			procCost += C[k].procCost;
			if(C[k].a > M)
				 M = C[k].a;
//			printf("C[%d].a=%d\n",k,C[k].a);
		}
		solM[i]=1.0*M;
		solC[i]=procCost;
		solOF[i] = 0.5*min_M/solM[i] + 0.5*min_procCost/solC[i];
//		printf("pop#%d:solM=%g, solC=%g, solOF=%g \n", i,solM[i],solC[i],solOF[i]);
//		getch();
	}
	
	// Selecting the best solution as final
	maxOF=0;
	for(i=0; i<npop; i++) {
		solOF[i] = 0.5*min_M/solM[i] + 0.5*min_procCost/solC[i];
//		printf("pop#%d:solM=%g, solC=%g, solOF=%g \n", i,solM[i],solC[i],solOF[i]);
		if(solOF[i] > maxOF) {
			maxOF = solOF[i];
			index = i;
//			printf("Updating is done!\n");
//			printf("maxOF=%g\n",maxOF);
//			printf("index=%d\n",index);
		}
	}
	// creating the final solution 
//	printf("\n*********************************\n");
//	printf("Final solution....\n");
	for (k=0; k<nfog; k++) {
		F[k].a = 0;
		F[k].procCost = 0;
	}
	for (k=0; k<ncloud; k++) {
		C[k].a = 0;
		C[k].procCost = 0;
	} 
	procCost=0;
	M=0;
//	for (j=0; j<ntask; j++)
//		printf("%4d",sol[index][j]);
//	printf("\n");
//	printf("index=%d\n",index);
//	printf("pop#%d:solM=%g, solC=%g, solOF=%g \n", index,solM[index],solC[index],solOF[index]);
//	getch();
	for (j=0; j<ntask; j++) {
		x = sol[index][j];
		if (x<nfog) {
			etime = (float)T[j].s / F[x].c * 1000;
			F[x].a += etime; 
			T[j].resp = F[x].a + F[x].d;
			F[x].procCost += ((float)T[j].s/F[x].c*F[x].pc);
//			printf("F[%d].a=%d\n",F[x].id, F[x].a);
//			getch();
		}
		else {
			x = x%nfog;
			etime = (float)T[j].s/C[x].c*1000;
			C[x].a += etime;
			T[j].resp = C[x].a + C[x].d;
			C[x].procCost += ((float)T[j].s/C[x].c*C[x].pc);
//			printf("C[%d].a=%d\n",C[x].id, C[x].a);
//			getch();
		}
	}
//	printf("\n----------Results----------\n");
	violationCost = 0;
	PDST=0;
	procCost=0;
	M=0;
	for(i=0; i<ntask; i++) {
//		printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
		temp = T[i].resp - T[i].d;
		if (temp < 0)
			temp = 0;
//		printf("temp = %g\n",temp);
		tempq = temp*100/T[i].d - 100 + T[i].q;
		if (tempq>0)
			violationCost += (tempq * T[i].p);
//		printf("total violationCost: %g\n\n",violationCost);
//		getch();	
		if (T[i].resp > T[i].d) 
			totalPenalty += T[i].resp - T[i].d;
		else
			PDST++;	
	}
	// calculating makespan and degree of imbalance (DI)
	int sumExe=0, avgExe, minExe = 10000;
	float DI; // in ms
	for(i=0; i<nfog; i++) {
		if(F[i].a > M)
			M = F[i].a;
		if(F[i].a < minExe)
			minExe = F[i].a;
		sumExe += F[i].a;
//		printf("F[%d].a=%d\n",i,F[i].a);
	}

	for(i=0; i<ncloud; i++) {
		if(C[i].a > M)
			M = C[i].a;
		if(C[i].a < minExe)
			minExe = C[i].a;
		sumExe += C[i].a;
//		printf("C[%d].a=%d\n",i,C[i].a);
	}
	avgExe = sumExe / (nfog+ncloud);
	DI = 1.0*(M-minExe) / avgExe;
	
	for(i=0; i<nfog; i++) {
		F[i].eng = F[i].a/1000.0*F[i].pmax + (M/1000.0-F[i].a/1000.0)*F[i].pmin; //ws or j
		engCons += 	F[i].eng;
		procCost += F[i].procCost;	
	}
	for(i=0; i<ncloud; i++) {
		C[i].eng = C[i].a/1000.0*C[i].pmax + (M/1000.0-C[i].a/1000.0)*C[i].pmin; //ws or j
		engCons += 	C[i].eng;
		procCost += C[i].procCost;	
	}
	//calculating min_engCons, min_procCost, min_M
	totTaskSize=0;
	totNodeCPU=0;
	for (i=0; i<ntask; i++)
		totTaskSize += T[i].s;
	for (i=0; i<nfog; i++)
		totNodeCPU += F[i].c;
	for (i=0; i<ncloud; i++)
		totNodeCPU += C[i].c;
	min_M = 1.0*totTaskSize / totNodeCPU; // in second
	if (f_max_pe > c_max_pe) {
		min_engCons = 1.0*totTaskSize/F[index_f_max_pe].c*F[index_f_max_pe].pmax;
//		printf("F[%d] is the most power eff. node\n",F[index_f_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*F[index_f_max_pe].pmax;
	}
	else {
		min_engCons = 1.0*totTaskSize/C[index_c_max_pe].c*C[index_c_max_pe].pmax;
//		printf("C[%d] is the most power eff. node\n",C[index_c_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*C[index_c_max_pe].pmax;
	}		
	if (f_max_ce > c_max_ce) {
//		printf("totTaskSize=%d , index=%d , MIPS=%d , procCost=%0.2f\n",totTaskSize,index_f_max_ce,F[index_f_max_ce].c,F[index_f_max_ce].pc);
		min_procCost = 1.0*totTaskSize/F[index_f_max_ce].c*F[index_f_max_ce].pc;
//		printf("F[%d] is the most cost eff. node\n",F[index_f_max_ce].id);
	}	
	else {
		min_procCost = 1.0*totTaskSize/C[index_c_max_ce].c*C[index_c_max_ce].pc;
//		printf("C[%d] is the most cost eff. node\n",C[index_c_max_ce].id);
	}
//	printf("\n");
//	printf("totTaskSize=%d\n",totTaskSize);
//	printf("totNodeCPU=%d\n",totNodeCPU);
//	printf("min_engCons=%.2f , min_procCost=%.2f , min_M=%.2f\n",min_engCons,min_procCost,min_M);
	fitValue = a*min_engCons/engCons + b*min_procCost/procCost + c*min_M/M*1000;
//	printf("a*min_engCons/engCons=%f\n",a*min_engCons/engCons);
//	printf("b*min_procCost/procCost=%f\n",b*min_procCost/procCost);
//	printf("c*min_M/M=%f\n",c*min_M/M*1000);
//	printf("\nengCons: %.2f",engCons);
//	printf("\nprocCost: %.2f",procCost);
//	printf("\nMakespan: %.2f",M/1000.0);
//	printf("\nfitValue: %.2f",fitValue);
//	printf("\nDI: %.2f",DI);
//	printf("\ntotalPenalty: %d",totalPenalty/1000);
//	printf("\ntotal violationCost: %g",violationCost);
//	printf("\nPDST: %g\n\n",PDST/ntask);
//	getch();
	sumM += M;
	sumengCons += engCons;
	sumprocCost += procCost;
	sumfitValue += fitValue;
	}
	printf("\n*************Final Reults*************\n");
	printf("\nMakespan: %.2f",sumM/1000.0/nrun);
	printf("\nengCons: %.2f",sumengCons/nrun);
	printf("\nprocCost: %.2f",sumprocCost/nrun);
	printf("\nfitValue: %.2f\n",sumfitValue/nrun);
}

/////////////////////////  Proposed  ////////////////////////////////
void Proposed(struct task task[],struct node fog[],struct node cloud[],int ntask,int nfog,int ncloud)
{
	float sumM=0, sumengCons=0, sumprocCost=0, sumfitValue=0;
	for (r=1; r<=nrun; r++) {
//	printf("\nRun=%d\n",r);
	struct task T[max];
	struct node F[max], C[max];
	tasks(ntask);
	nodes(nfog, ncloud);
	int M=0, totalPenalty=0, minE, flag; // M:Makespan, minE:Minimum_Executiontime
	int totTaskSize=0, totNodeCPU=0; //totTaskSize: size of all tasks totNodeCPU: CPU power of all nodes
	float f_max_pe=0, f_max_ce=0, c_max_pe=0, c_max_ce=0, min_ms=100000000, max_fit=0;
	float procCost=0, engCons=0, fitValue=0;
	float min_engCons=0, min_procCost=0, min_M=0; //min_M: min. makespan
	float a=0.25, b=0.25, c=0.5; //coefficients a: eng, b: cost, c: makespan
	for (i=0; i<ntask; i++) {
		T[i] = task[i];
		T[i].resp = 0;
	}
	for (i=0; i<nfog; i++) {
		F[i] = fog[i];
		F[i].a = 0;
		F[i].procCost = 0;
		F[i].eng = 0;
	}
	for (i=0; i<ncloud; i++) {
		C[i] = cloud[i];
		C[i].a = 0;
		C[i].procCost = 0;
		C[i].eng = 0;
	} 	
//	SortTasks(T,ntask);
//	for (i=0; i<ntask; i++) 
//		printf("%d\t",T[i].id);
//	printf("\n----------Scheduling----------\n");
	//calculating the power and cost efficiency of nodes
	for (i=0; i<nfog; i++) {
		F[i].pe = F[i].c/F[i].pmax;
		F[i].ce = F[i].c/F[i].pc;
		if (F[i].pe > f_max_pe) {
			f_max_pe = F[i].pe;
			index_f_max_pe = i;
		}
		if (F[i].ce > f_max_ce) {
			f_max_ce = F[i].ce;
			index_f_max_ce = i;
		}
//		printf("F[%d].pe=%-8.2f, F[%d].ce=%-8.2f\n",F[i].id,F[i].pe,F[i].id,F[i].ce);
	}
//	printf("\nf_max_pe=%-8.2f  ,  f_max_ce=%-8.2f\n\n",f_max_pe, f_max_ce);
	for (i=0; i<ncloud; i++) {
		C[i].pe = C[i].c/C[i].pmax;
		C[i].ce = C[i].c/C[i].pc;
		if (C[i].pe > c_max_pe) {
			c_max_pe = C[i].pe;
			index_c_max_pe = i;
		}	
		if (C[i].ce > c_max_ce) {
			c_max_ce = C[i].ce;
			index_c_max_ce = i;
		}	
//		printf("C[%d].pe=%-8.2f, C[%d].ce=%-8.2f\n",C[i].id,C[i].pe,C[i].id,C[i].ce);
	}
//	printf("\nc_max_pe=%-8.2f  ,  c_max_ce=%-8.2f\n\n",c_max_pe, c_max_ce);
	
	//normalization of energy and cost efficieny of nodes
	for (i=0; i<nfog; i++) {
		F[i].pe /= f_max_pe;
		F[i].ce /= f_max_ce;
//		printf("F[%d].pe=%-8.2f  ,  F[%d].ce=%-8.2f\n",F[i].id,F[i].pe,F[i].id,F[i].ce);
	}
	for (i=0; i<ncloud; i++) {
		C[i].pe /= c_max_pe;
		C[i].ce /= c_max_ce;
//		printf("C[%d].pe=%-8.2f  ,  C[%d].ce=%-8.2f\n",C[i].id,C[i].pe,C[i].id,C[i].ce);
	}
//	printf("\n");
//	getch();
	for (i=0; i<ntask; i++) {
//		printf("\nT[%d] is ready to be scheduled!\n",T[i].id);
		flag = 0;
		max_fit=0;
		min_ms=100000000;
		for (j=0; j<nfog; j++) {
			etime = (float)T[i].s / F[j].c * 1000;
			F[j].ms = F[j].a + etime; 
//			printf("F[%d].ms=%.2f\n",F[j].id, F[j].ms);
			if (F[j].ms < min_ms)
				min_ms=F[j].ms;	
		}
		for (j=0; j<ncloud; j++) {
			etime = (float)T[i].s / C[j].c * 1000;
			C[j].ms = C[j].a + etime; 
//			printf("C[%d].ms=%.2f\n",C[j].id, C[j].ms);
			if (C[j].ms < min_ms) 
				min_ms=C[j].ms;
		}
//		printf("\nmin_ms = %.2f\n",min_ms);
//		getch();
		for (j=0; j<nfog; j++) {
			F[j].ms = min_ms/F[j].ms;
//			printf("F[%d].ms=%.2f\n",F[j].id, F[j].ms);
		}
		for (j=0; j<ncloud; j++) {
			C[j].ms = min_ms/C[j].ms;
//			printf("C[%d].ms=%.2f\n",C[j].id, C[j].ms);
		}
//		getch();
		//calculating fitness value for nodes
//		printf("\n");
		for (j=0; j<nfog; j++) {
			F[j].fit = a*F[j].pe + b*F[j].ce + c*F[j].ms;
			if (F[j].fit > max_fit) {
				max_fit = F[j].fit;
				index = j;
				flag = 1;
			}	
//			printf("F[%d].fit=%.2f\n",F[j].id,F[j].fit);
		}
		for (j=0; j<ncloud; j++) {
			C[j].fit = a*C[j].pe + b*C[j].ce + c*C[j].ms;
			if (C[j].fit > max_fit) {
				max_fit = C[j].fit;
				index = j;
				flag = 2;
			}
//			printf("C[%d].fit=%.2f\n",C[j].id,C[j].fit);
		}
//		getch();
		// assigning phase
//		printf("flag=%d\n",flag);
		if (flag==1) {
			F[index].a += (float)T[i].s/F[index].c*1000;
			T[i].resp = F[index].a + F[index].d;
			F[index].procCost += ((float)T[i].s/F[index].c*F[index].pc);
//			printf("T[%d] --> F[%d]\n", T[i].id, F[index].id);
//			printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//			printf("F[%d].a is: %d\n\n",F[index].id,F[index].a);
//			getch();	
		}
		if (flag==2) {
			C[index].a += (float)T[i].s/C[index].c*1000;
			T[i].resp = C[index].a + C[index].d;
			C[index].procCost += ((float)T[i].s/C[index].c*C[index].pc);
//			printf("T[%d] --> C[%d]\n", T[i].id, C[index].id);
//			printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
//			printf("C[%d].a is: %d\n\n",C[index].id,C[index].a);
//			getch();	
		}
	}
//	getch();
//	printf("\n----------Results----------\n");
	violationCost = 0;
	PDST=0;
	for(i=0; i<ntask; i++) {
//		printf("T[%d].deadline= %4d  T[%d].response= %4d\n",T[i].id, T[i].d, T[i].id, T[i].resp);
		temp = T[i].resp - T[i].d;
		if (temp < 0)
			temp = 0;
//		printf("temp = %g\n",temp);
		tempq = temp*100/T[i].d - 100 + T[i].q;
		if (tempq>0)
			violationCost += (tempq * T[i].p);
//		printf("total violationCost: %g\n\n",violationCost);
//		getch();	
		if (T[i].resp > T[i].d) 
			totalPenalty += T[i].resp - T[i].d;
		else
			PDST++;	
	}
	// calculating makespan and degree of imbalance (DI)
	int sumExe=0, avgExe, minExe = 10000;
	float DI; // in ms
	for(i=0; i<nfog; i++) {
		if(F[i].a > M)
			M = F[i].a;
		if(F[i].a < minExe)
			minExe = F[i].a;
		sumExe += F[i].a;
//		printf("F[%d].a=%d\n",i,F[i].a);
	}

	for(i=0; i<ncloud; i++) {
		if(C[i].a > M)
			M = C[i].a;
		if(C[i].a < minExe)
			minExe = C[i].a;
		sumExe += C[i].a;
//		printf("C[%d].a=%d\n",i,C[i].a);
	}
	avgExe = sumExe / (nfog+ncloud);
	DI = 1.0*(M-minExe) / avgExe;
	for(i=0; i<nfog; i++) {
//		printf("F[%d].a=%d\n",F[i].id, F[i].a);
		F[i].eng = F[i].a/1000.0*F[i].pmax + (M/1000.0-F[i].a/1000.0)*F[i].pmin; //ws or j
//		printf("F[i].eng=%g\n",F[i].eng);
//		getch();
		engCons += 	F[i].eng;
		procCost += F[i].procCost;	
	}
	for(i=0; i<ncloud; i++) {
		C[i].eng = C[i].a/1000.0*C[i].pmax + (M/1000.0-C[i].a/1000.0)*C[i].pmin; //ws or j
		engCons += 	C[i].eng;
		procCost += C[i].procCost;	
	}
	//calculating min_engCons, min_procCost, 
	for (i=0; i<ntask; i++)
		totTaskSize += T[i].s;
	for (i=0; i<nfog; i++)
		totNodeCPU += F[i].c;
	for (i=0; i<ncloud; i++)
		totNodeCPU += C[i].c;
	min_M = 1.0*totTaskSize / totNodeCPU; // in second
	if (f_max_pe > c_max_pe) {
		min_engCons = 1.0*totTaskSize/F[index_f_max_pe].c*F[index_f_max_pe].pmax;
//		printf("F[%d] is the most power eff. node\n",F[index_f_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*F[index_f_max_pe].pmax; //ws or j
	}
	else {
		min_engCons = 1.0*totTaskSize/C[index_c_max_pe].c*C[index_c_max_pe].pmax;
//		printf("C[%d] is the most power eff. node\n",C[index_c_max_pe].id);
//		min_engCons = min_M*(nfog+ncloud)*C[index_c_max_pe].pmax;
	}		
	if (f_max_ce > c_max_ce) {
//		printf("totTaskSize=%d , index=%d , MIPS=%d , procCost=%0.2f\n",totTaskSize,index_f_max_ce,F[index_f_max_ce].c,F[index_f_max_ce].pc);
		min_procCost = 1.0*totTaskSize/F[index_f_max_ce].c*F[index_f_max_ce].pc;
//		printf("F[%d] is the most cost eff. node\n",F[index_f_max_ce].id);
	}	
	else {
		min_procCost = 1.0*totTaskSize/C[index_c_max_ce].c*C[index_c_max_ce].pc;
//		printf("C[%d] is the most cost eff. node\n",C[index_c_max_ce].id);
	}
//	
//	printf("totTaskSize=%d\n",totTaskSize);
//	printf("totNodeCPU=%d\n",totNodeCPU);
//	printf("min_engCons=%.2f , min_procCost=%.2f , min_M=%.2f\n",min_engCons,min_procCost,min_M);
	fitValue = a*min_engCons/engCons + b*min_procCost/procCost + c*min_M/M*1000;
//	printf("a*min_engCons/engCons=%f\n",a*min_engCons/engCons);
//	printf("b*min_procCost/procCost=%f\n",b*min_procCost/procCost);
//	printf("c*min_M/M=%f\n",c*min_M/M*1000);
//	printf("\nengCons: %.2f",engCons);
//	printf("\nprocCost: %.2f",procCost);
//	printf("\nMakespan: %.2f",M/1000.0);
//	printf("\nfitValue: %.2f",fitValue);
//	printf("\nDI: %.2f",DI);
//	printf("\ntotalPenalty: %d",totalPenalty/1000);
//	printf("\ntotal violationCost: %g",violationCost);
//	printf("\nPDST: %g\n",PDST/ntask);
//	getch();
	sumM += M;
	sumengCons += engCons;
	sumprocCost += procCost;
	sumfitValue += fitValue;
	}
	printf("\n*************Final Reults*************\n");
	printf("\nMakespan: %.2f",sumM/1000.0/nrun);
	printf("\nengCons: %.2f",sumengCons/nrun);
	printf("\nprocCost: %.2f",sumprocCost/nrun);
	printf("\nfitValue: %.2f\n",sumfitValue/nrun);
}

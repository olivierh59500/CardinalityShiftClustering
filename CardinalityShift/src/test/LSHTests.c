#include "../LSHDecoder.c"
#include "../leechArrayDecoder.c"
#include "../e8Decoder.c"
#include "../QAM16Decoder.c"
#include "testUtils.c"

#define NULL 0

/*
 * As a sanity check for lattice decoding, generate a bunch of random
 * vectors and store their hashes. At set intervals output the number of
 * unique hashes. This should be approximately min distance of the lattice
 * less ratio than the set of all possible permutations.
 */
void countNZ()
{
   int j,k;
   long i,total=0;

   unsigned char* allhashes = malloc(16777216);
   printf("OK\n");

   //zero it out
   for(i=0;i <16777216*32;i++ )allhashes[i]=0;
   printf("OK\n");

   float r[12][2];
   float dist;
   unsigned long ret1;

    for (i=0;i<16777216*16;i++)
    {


           for(j=0;j<12;j++){
               r[j][0]=8*((float)rand())/RAND_MAX;
               r[j][1]=8*((float)rand())/RAND_MAX;
           }

            ret1 = q.decode(r,&dist);

            //ret1 = ELFhash(&ret1, 16777216*32);

           if(allhashes[ret1]  ==0)total++;
           allhashes[ret1]++;
           if(i %100000==0)
           {

               printf("%d:%d\n", i,total);

           }

       }
}

/*
 * Test the probability of intersection between two arbitrary length vectors'
 *  elf+leech hashes. Store probabilities in histogram buckets indexed by
 * the real distance between them vectors.
  */
void testRatios2(int tests , float bias, int * counts, int * totals, float * buckets,int len){

  int i,j , k;

      for (i=0;i<tests;)
        {
          float dist;
          float* V = malloc(sizeof(float)*len);
          genRandomVector(len,1.0,V);
          float* Q = malloc(sizeof(float)*len);

          for(j=0;j<len;j++)
            {
              Q[j]=V[j]  +   bias*(-1+2*((float)rand())/RAND_MAX);
          }
          dist = testDist(V,Q,len);

          int b = 0;
          while(dist>buckets[b++]);

          totals[b]++;
          float* M = GenRandomN(len,24);


          unsigned long ret1 = lshHash(V, len, 1, 1671711, M,&distance);

          int t = 0;
          for(k=0;k<16 && t==0;k++)
            t =(int)(lshHash(Q, len, 1, 1671711,M, &distance) == ret1);

            counts[b]+= t;

            i++;

      }
}




/*

 //test unencoded
unsigned long ret = 0;

int m= 0 ;
for(;m<12;m++)
  {
    if(r[m][0]>=6)ret = (ret<<2)+3;
    else if(r[m][0]>=4)ret = (ret<<2)+2;
    else if(r[m][0]>=2)ret = (ret<<2)+1;
    else ret = ret<<2;

    if(r[m][1]>=6)ret = (ret<<2)+3;
    else if(r[m][1]>=4)ret = (ret<<2)+2;
    else if(r[m][1]>=2)ret = (ret<<2)+1;
    else ret = ret<<2;

  }
return ret;
 */



/*
 * test the linearity of random projections of vectors'
 * distances and the actual distances.
 */
void testProjection(int tests , int len){
 int i,j,b=(int)((float)len/(float)6);
 float* q =malloc(sizeof(float)*len) ;
 float* v=malloc(sizeof(float)*len) ;

float * full =malloc(sizeof(float)*tests) ;
float * projs = malloc(sizeof(float)*tests);



 float* r1= malloc(sizeof(float)*24);
 float* r2= malloc(sizeof(float)*24);


 float dist,distSub;

 int  *R = malloc(sizeof(int)*24*b*2);;

 float* M = GenRandomN(len,24);

 float avgRatio=0.0;
 float randn = GenRandom(len,24,R);
     for (i=0;i<tests; i++)
       {
         genRandomVector(len,1.0,v);
         for(j=0;j<len;j++)q[j] = v[j]+sampleNormal()*(((float)i)/((float)tests));
         project(v,r1,R,randn, len,24);
         lshHash(v,len,1,100000,M,&dist);

         project(q,r2,R,randn, len,24);

         dist = testDist(v,q,len);
         distSub = testDist(r1,r2,24);

         full[i]=dist;
         projs[i] = distSub;
     }

     free(r1);
     free(r2);
     free(R);
     free(v);
     free(q);

     histogram(30,len, full,projs);

     free(full);
     free(projs);


}



void avgDistTest(){
/*
  * Another Test Case for average distance independent of dimensionality
  */

 int dim = 100;
 int points = 3000;
 int i=3;
 for(i=1;i<6;i++)
    testProjection(points ,1000*i);

 return;
 /*
  unsigned long d ,originalD= lshHash(V, len, times, 1671711, &distance);
  //print(prevD,9);
  //for(i=0;i<len;i++)V[i]=Q[i];


  char* t;

  float c[24];
  float * R =malloc(len*sizeof(float));


  int ct = 0;
  //printVecF(V,len);

  while(d!=originalD){
      d = lshHash(V, len, times, 1671711, &distance);
     ct++;
  }

  //printf("d1: %f,\n",testDist(V,Q,len));
  printf("t1: %d\n",ct);

  //print(d,9);


  genRand(len,1.0,Q);
  //printVecF(Q,40);
  //printf("d2: %f,\n",testDist(V,Q,len));


  d = 0;
  ct = 0;

     while(d!=originalD){
         d = lshHash(Q, len, times, 1671711, &distance);
        ct++;
     }
     //printf("t2: %d\n",ct);


  //print(d,9);*/

}


void leechTests(){

      //some more samples
      //
      //{{7.0,3.0},{3.0,3.0},{7.0,7.0},{3.0,3.0},
      //    {7.0,7.0},{7.0,7.0},{3.0,7.0},{7.0,7.0},
      //    {5.0,5.0},{5.0,1.0},{5.0,5.0},{5.0,5.0}}

      //{{1.0,1.0},{3.0,7.0},{7.0,3.0},{5.0,5.0},
          //{7.0,7.0},{5.0,5.0},{1.0,5.0},{3.0,7.0},
          //{1.0,1.0},{7.0,7.0},{7.0,7.0},{1.0,1.0}}

      //{{5.0,7.0},{3.0,5.0},{1.0,3.0},{7.0,1.0},
          //{3.0,5.0},{5.0,7.0},{3.0,1.0},{5.0,3.0},
          //{1.0,3.0},{3.0,5.0},{1.0,3.0},{7.0,1.0}}

      float dist;
      int i;
      float r1[12][2] ={{7.0,7.0},{1.0,5.0},{1.0,1.0},{3.0,3.0},
                          {5.0,1.0},{3.0,7.0},{3.0,7.0},{5.0,5.0},
                          {3.0,3.0},{1.0,1.0},{5.0,5.0},{3.0,7.0}};
                      //{{7.0, 5.0}, {3.0, 1.0}, {3.0, 1.0}, {3.0, 1.0}
                      //, {1.0, 3.0}, {1.0, 3.0}, {1.0, 7.0}, {5.0, 3.0},
                      //  {7.0, 5.0}, {7.0, 5.0}, {7.0, 1.0}, {7.0, 1.0}};
      for(i=0;i<12;i++){r1[i][0]=(r1[i][0]/4.0)-1.0;r1[i][1]=(r1[i][1]/4.0)-1.0;}

      print(decodeLeech(r1,&dist),12,3);printf("%f\n",dist);

      long ret = 0;

      //for(i = 0;i<36;i++)ret = decode(r1,&dist)[i] +(ret <<1) ;
      //print(ret,9);


      //print(decode(r1,&dist),9);
      //decode(r1,&dist);
      //add some noise
      r1[0][1]=r1[0][1]-.45;
      r1[3][0]=r1[3][0]+.25;
      r1[5][1]=r1[5][1]+.31;
      r1[7][1]=r1[7][1]-.1;
      r1[10][1]=r1[10][1]+.20;
      r1[11][1]=r1[11][1]+.40;


      //print(decode(r1,&dist),9);printf("%f\n",dist);
      print(decodeLeech(r1,&dist),12,3);printf("%f\n",dist);

      //ret = 0;

      //for(i = 0;i<36;i++)ret = decode(r1,&dist)[i] +(ret <<1) ;
      //print(ret,9);

      printf("\n");
      dist = 0;
      float r2[12][2] =    {{7.0,3.0},{3.0,3.0},{7.0,7.0},{3.0,3.0},
          {7.0,7.0},{7.0,7.0},{3.0,7.0},{7.0,7.0},
          {5.0,5.0},{5.0,1.0},{5.0,5.0},{5.0,5.0}};
                      //{{3.0, 1.0}, {5.0, 7.0}, {7.0, 1.0}, {5.0, 7.0},
                      //  {5.0, 3.0}, {7.0, 5.0}, {1.0, 3.0}, {3.0, 1.0},
                      //  {7.0, 1.0}, {5.0, 3.0}, {7.0, 5.0}, {5.0, 3.0}};

      //print(decode(r2,&dist),9);printf("%f\n",dist);
      print(decodeLeech(r1,&dist),12,3);
      printf("%f\n",dist);
      ret = 0;
      //for(i = 0;i<36;i++)ret = (ret <<1) +decode(r2,&dist)[i];
      //print(ret,9);
          //add some noise

          r2[0][1]=r2[0][1]-1.5;
          r2[5][1]=r2[5][1]+7.1;
          r2[6][1]=r2[6][1]-2.1;
          r2[8][1]=r2[8][1]-1.1;
          r2[6][0]=r2[6][0]-2.1;
          r2[8][0]=r2[8][0]-2.1;

        //  print(decode(r2,&dist),9);printf("%f\n",dist);
          print(decodeLeech(r1,&dist),12,3);
          printf("%f",dist);
          ret = 0;
          //for(i = 0;i<36;i++)ret = (ret <<1) +decode(r2,&dist)[i];
          //print(ret,9);
          printf("\n");


      float r3[12][2] =
                      {{3.0, 3.0}, {3.0, 3.0}, {5.0, 1.0}, {5.0, 5.0},
                      {3.0, 7.0}, {3.0, 3.0}, {5.0, 1.0}, {1.0, 5.0},
                      {3.0, 7.0}, {3.0, 7.0}, {1.0, 5.0}, {5.0, 5.0}};

      //print(decode(r3,&dist),9);printf("%f\n",dist);
      print(decodeLeech(r1,&dist),12,3);
      printf("%f\n",dist);
          //add some noise
          r3[0][1]=r3[0][1]-2.5;
          r3[0][0]=0.0;
          r3[3][1]=r3[3][1]+1.51;
          r3[3][0]=r3[3][0]+1.51;
          r3[11][0]=r3[11][0]-1.1;
          r3[11][1]=r3[11][1]-1.1;
          //decode(r3,&dist);
          //print(decode(r3,&dist),9);printf("%f\n",dist);
          print(decodeLeech(r1,&dist),12,3);
          printf("%f",dist);
          printf("\n");


      float r4[12][2] ={{1.0,1.0},{3.0,7.0},{7.0,3.0},{5.0,5.0},
                        {7.0,7.0},{5.0,5.0},{1.0,5.0},{3.0,7.0},
                        {1.0,1.0},{7.0,7.0},{7.0,7.0},{1.0,1.0}};
                      //{{5.0, 1.0},{3.0, 3.0},{1.0, 1.0},{3.0, 7.0},
                      //{1.0, 1.0},{7.0, 7.0}, {5.0, 5.0}, {3.0, 3.0},
                      //{1.0, 1.0}, {7.0, 3.0}, {1.0, 1.0}, {3.0, 7.0}};

          //print(decode(r4,&dist),9);printf("%f\n",dist);
      print(decodeLeech(r1,&dist),12,3);
          printf("%f\n",dist);
          //add some noise
          r4[0][0]=r4[0][0]-1.3;
          r4[1][1]=r4[1][1]+.4;
          r4[2][1]=r4[2][1]-1.3;
          r4[3][0]=r4[3][0]-3.1;
          r4[4][1]=r4[4][1]+2.1;


          r4[11][0]=r4[11][0]-2.1;


          //print(decode(r4,&dist),9);printf("%f\n",dist);
          print(decodeLeech(r1,&dist),12,3);
          printf("%f",dist);



}

void e8Test(){
      srand((unsigned int)2141331);
      int i,j;
      float* e = malloc(sizeof(float)*8);

      for(j=0;j<100;j++){
        genRandomVector(8,1.0,e);
        for(i = 0 ; i<8;i++)e[i] = e[i]+1;
        //printVecF(e,8);
        float * dist;
        print(decodeE8(e,dist),24,1);
      }
}

void countNZHistoGram(){
      int i;
      int j,p;
       float k = 30/((float)p);
       float buckets[30] = {0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0 };
       for(i=0;i<p;i++)buckets[i] = ((float)i)*k;
       int len = 100;

       int counts[30]= {0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0 };
       int totals[30]= {0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0 };


       for(i=1;i<100;i++){
           testRatios2(100 , 1 , & counts, & totals,  buckets,len);
           printf("test: %f\n",i*.1);
       }

           for (i=0;i<p;i++)printf("%f,",buckets[i]);
           printf("\n");
           for (i=0;i<p;i++)printf("%f,",((float)counts[i])/((float)totals[i]));
           printf("\n");
           for (i=0;i<p;i++)printf("%d,",totals[i]);
           printf("\n");
}

/*
 * This tests projects a point using the various random normal distribution projection
 * methods, and records the probability of an intersection. This is the search complexity
 * Pr() required to maintain a database size that is linear with the dataset. ie time-space
 * tradeoff, O(nlogn) - space, => n-space, but O(log(n)n^p) - complexity,
 * NOTE: O(n^p) dominates.
 */
void testRandProjectionIntersectPr(){
  int i;
  int len = 10000;
  float* V = malloc(sizeof(float)*len);
  genRandomVector(len,1.0,V);
  float* Q = malloc(sizeof(float)*len);
  float distance = 0;
  int dim = 8;
  int times = 2;

  unsigned long list[100];
  float* R = GenRandomN(len,24);


  for(i=0;i<14;i++){
    genRandomVector(len,1.0,V);
    //printVecF(V,len);
    //printf(">");printVecF(Q,len);
    unsigned long q = lshHash(V,len,1,100000,R,&distance);
    int loc = listSearchAndAdd(q, list,i);
    if(loc){
      ;//printf("%i\n",loc);

    }
  }

}
/*
 * Here we will test secondary hash functions for overall entropy (hint we want about 1 per bucket)
 */
#define MULTI 1
void testHASH(int tablesize){
  unsigned int i = 0;
  float * list = malloc(sizeof(unsigned int) * tablesize);
  float * range = malloc(sizeof(unsigned int) * tablesize);


  float* V = malloc(sizeof(float)*5000);

  float *R = GenRandomN(5000,24);

  Quantizer * q= initializeQuantizer(decodeLeech, 24);
  initLSH(q);

  float d;

  for(;i<tablesize;i++)
    {
      list[i]=0;
      range[i] = i;
    }

  unsigned long l = 0L;


  for(i=0;i<tablesize;i++)
    {
      if(i%(100*MULTI)==0)printf("%.1f\n",((float)i)/(100*MULTI));
      genRandomVector(5000,1.0,V);
      l = lshHash(V, 5000, 1, tablesize, R,&d);
      list[l]++;

  }

  histogram(50,tablesize, range,list);

}



/*
 * Test the probability of intersection between two leech hashed vectors
 * in 24 dimensions. Store probabilities in histogram buckets indexed by
 * the real distance between them vectors.
  */
void testRatios(int tests, int l, int dim){


  Quantizer * q= initializeQuantizer(decodeLeech, 24);
  initLSH(q);


  float* R = GenRandomN(dim,24);

  float bias ;
  int i,j,b,k;
  float *r = malloc(sizeof(float)*dim);
  float *p= malloc(sizeof(float)*dim);

  float* buckets = malloc(l*sizeof(float));
  int* counts = malloc(l*sizeof(int));
  int* totals = malloc(l*sizeof(int));

  for(i=0;i<l;i++)
    {
            buckets[i] = i*.25;
            counts[i]=0;
            totals[i]=0;
    }

  for(k=0;k<100;k++)
  {
            bias = k*.005;


              for (i=0;i<tests;)
                {
                        float dist;
                        for(j=0;j<dim;j++){
                            r[j]=2.0*((float)rand())/RAND_MAX -1;
                            p[j]=r[j]   +   bias*(sampleNormal());
                         }

                          b = 0;
                          dist = testDist(r,p,dim);
                          while(dist>buckets[b++] &&b<l);

                          long ret1 =  lshHash(r, dim, 1, tests, R, &dist);


                          long ret2;
                          int trs = 0;
                          //10 is log(# tests) for now
                          for (;trs<15;trs++){
                              ret2=lshHash(p, dim, 1, tests, R, &dist);
                              if(ret1==ret2){
                                  counts[b]++;
                                  trs = 15;
                              }

                          }

                          totals[b]++;
                          i++;
              }
  }

  /*or(i=0;i<l;i++)printf("%.2f, ",buckets[i]);printf("\n");
  for(i=0;i<l;i++)printf("%i, ",counts[i]);  printf("\n");
  for(i=0;i<l;i++)printf("%i, ",totals[i]);  printf("\n");*/
}

/*
 * This test projects points until relatively large buckets of points begin to emerge.
 * Serving as an attempt to prove the hypothesis that these points are the approx.
 * density modes of the later discovered clusters.
 * max number of tests, l is the target bucket size before emitting to the parallel system
 * dim is the vector dimensionality
 */
void big_bucket_Search(int numTests, int part, int dim,int cutoff)
{
  Quantizer * q= initializeQuantizer(decodeLeech, 24);
  initLSH(q);
  //Quantizer * q= initializeQuantizer(decodeQAM16,2);
  //initLSH(q);

  int hashMod = 10000;
  float *r = malloc(sizeof(float)*dim);

  int clu = 7;
  int d = 24;//q.dimensionality;


  float* clusterCenters=  generateRandomCenters(dim,clu) ;
  float* ret=generateGaussianClusters(part,dim,clu,clusterCenters);

  int i,k,j=0;

  for(i=0;i<clu;i++)
  {
      printVecF(&clusterCenters[i*dim],dim);
      printf("\n");
  }

  printf("--------------------------------------------------------\n");


  //size of the buckets
  int* buckets = malloc(sizeof(int)*hashMod);

  //bucket centroid
  float* bucketsAvgs = malloc(sizeof(float)*hashMod*dim);

  //sum of decoding distances
  float* distances = malloc(sizeof(float)*hashMod);

  //pass by reference
  float distance;



    for(;j<numTests;j++){

      float* M = GenRandomN(d,dim);
      for(i=0;i<clu*part;i++)
      {
          long hash = lshHash(&ret[i*dim],dim, 1, hashMod,M, &distance);
          //printf("%i:%c\n",hash,(char)(((float)i)/((float)l))+64);
          //accumulate hits to this bucket
          buckets[hash]++;
          distances[hash]+=distance;
          //compute a moving average for current bucket
          //m_{i+1} = (m_{i}+v)/(n+1)
          for(k=0;k<dim;k++)
            bucketsAvgs[hash*dim+k]=(bucketsAvgs[hash*dim+k]*((float)buckets[hash])+ret[i*dim+k])/(((float)buckets[hash])+1.0);



      }
      free(M);
  }



  int *centroidIdx = malloc(sizeof(int)*cutoff);

  //initialize the top cutoff buckets
  for(i=0;i<cutoff;i++)centroidIdx[i] = buckets[i];

  //find the top k
  for(i=cutoff;i<hashMod;i++)
  {
      int argleast = 0;
      int sizeofCandidate = buckets[i];




      for(k=0;k<cutoff;k++)
      {
          //checks for biggest buckets
          if(  sizeofCandidate >  buckets[centroidIdx[k]] ){
            //then the kth bucket can be replaced
            //but we need to also keep looking if
            //something is even less
            sizeofCandidate = buckets[centroidIdx[k]];
            argleast = k;//this is getting replaced
          }
      }




      //after the least is found it should be in argleast
      if( buckets[i] >buckets[centroidIdx[argleast]] ){
          /*
          char booltest = 0;
          for(k=0;k<cutoff;k++)
          {
              //can probably make this more efficient, but im sleepy so for now lets just loop
              //again and check for overlaps here
              if(testDist(&bucketsAvgs[i*dim], &bucketsAvgs[centroidIdx[k]*dim],dim)<1.2){
                  booltest = 1;
              }
          }

          if(booltest == 0)*/centroidIdx[argleast] = i;

      }


  }

  for(k=0;k<cutoff;k++)
    {
      printf("%i:%i: ",centroidIdx[k],buckets[centroidIdx[k]]);
      printVecF(&bucketsAvgs[dim*centroidIdx[k]],dim);
      printf("\n ");
    }
  printf("\n");




  float * centroids = malloc(sizeof(float)*dim*k);


  free(buckets);
  free(distances);
  free(r);
  free(clusterCenters);
  free(ret);
}



void printRandomClusters(){

  int i;
  int part= 10;
  int clu = 8;
  int d = 3;


  float* clusterCenters=  generateRandomCenters(d,clu) ;
  float* ret=generateGaussianClusters(part,d,clu,clusterCenters);
  float* M = GenRandomN(d,d);


  printVecF(M,9);

  printf("s=[");
  for (i=0;i<clu*part;i++){
      projectN(&ret[i*d],  &ret[i*d],M, d,d);
      printf("[");
      printVecF(&ret[i*d],d);
      printf("],");


  }
  printf("]\n\n\na=[");
  for (i=0;i<clu;i++){
      projectN(&clusterCenters[i*d],  &clusterCenters[i*d],M, d,d);
      printf("[");
      printVecF(&clusterCenters[i*d],d);
      printf("],");
  }

  printf("]\n");
  free(clusterCenters);
  free(ret);

}



int main(int argc, char* argv[])
{
  srand((unsigned int)2141331);
  //printRandomClusters();
  //                              trials, pts in a cluster, dimensions, bucket cutosff
  big_bucket_Search(20, 1000,100,30);
  //testRatios(2000,20,100);
  //leechTests();
  //testHASH(10000*MULTI);

  return 0;
}


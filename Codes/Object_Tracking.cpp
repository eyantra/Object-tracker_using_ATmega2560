
//convention followed is row column not graph wise

#include<iostream>
#include<cv.h>
#include<C:\Program Files\OpenCV\otherlibs\highgui\highgui.h>
#include<stdlib.h>
#include<iomanip>
#include<fstream>
#include<math.h>

using namespace std;

int p=0,x_1,y_1,x_2,y_2,x_3,y_3,x_4,y_4;     //boundary points
int k=10;                                    //number of neurons
int *sigma;                                  //sigma values
CvScalar* u;                                 // myu values          
double *alpha;                               //alpha values
double *Y;                                   //Y values either takes 1 or -1
int x_centroid=0,y_centroid=0;               //reference centroid
int no_of_pixels=0;
int original_count=0;
int temp_count=0;

int x_centroid_temp=0,y_centroid_temp=0;     //instantaneous centroid
int x_prev_centroid=0,y_prev_centroid=0;
int no_of_pixels_temp=0;

int top_c=0,down_c=0,left_c=0,right_c=0;     //rectangle properties w.r.t centroid 
int l_approx = 100;		//distance in cm


//function for calculating distance
float cvCalcDist(int i,int j,int k,int l)
{      
    return sqrt( pow((i-k)*1.0,2) + pow((j-l)*1.0,2));     
}


//function to calculate distance between two CvScalars
float diff(CvScalar s1,CvScalar s2)
 {
    return sqrt(pow((s1.val[0]-s2.val[0]),2) + pow((s1.val[1]-s2.val[1]),2) + pow((s1.val[2]-s2.val[2]),2));
 }

//function to select object and draw rectangle 
void get_coordinate(int event,int y,int x,int flags,void* param)
{
     if(p==2)
     {
        cvRectangle(param,cvPoint(y_1,x_1),cvPoint(y_2,x_2),cvScalarAll(255),1);    
        cout<<"rectangle drawn";

        double l,b,ldash,bdash;
        b=x_2-x_1+1;
        l=y_2-y_1;
        
        ldash=sqrt(2.0)*l;
        bdash=sqrt(2.0)*b;
        
        int dx,dy;
        
        dx=(int)(bdash-b)/2;
        dy=(int)(ldash-l)/2;
        
        x_3=x_1-dx;   y_3=y_1-dy;
        x_4=x_2+dx;   y_4=y_2+dy;
          
		//cvRectangle(param,cvPoint(y_3,x_3),cvPoint(y_4,x_4),cvScalarAll(255),1);    

		cvShowImage("Source",param); 
        p++;
    }

     if(event==CV_EVENT_LBUTTONUP && p<2)
     {
          if(p==1)
          {   
              x_2=x; y_2=y;
              p++;
              cout<<x<<" "<<y<<endl;
          } 
          if(p==0)
          {   
              x_1=x; y_1=y;
              p++;   
              cout<<x<<" "<<y<<endl;
          }
     }
}       

//function to get the color of a pixel
CvScalar cvGetColor(IplImage* temp_img,int i,int j)
{
     int height     = temp_img->height;
     int width      = temp_img->width;
     int step       = temp_img->widthStep/sizeof(uchar); 
     int channels   = temp_img->nChannels;
     uchar* data    = (uchar *)temp_img->imageData;
     
     CvScalar color;
     color.val[0]=data[i*step+j*channels+0];
     color.val[1]=data[i*step+j*channels+1];
     color.val[2]=data[i*step+j*channels+2];
     
     return color;
     }
     
//function to set the color of a pixel     
void cvSetColor(IplImage* temp_img,int i,int j,CvScalar color)
{
     int height     = temp_img->height;
     int width      = temp_img->width;
     int step       = temp_img->widthStep/sizeof(uchar); 
     int channels   = temp_img->nChannels;
     uchar* data    = (uchar *)temp_img->imageData;
     
     data[i*step+j*channels+0]=color.val[0];
     data[i*step+j*channels+1]=color.val[1];
     data[i*step+j*channels+2]=color.val[2];    
}   

//main function starts here
int main()
{
    time_t lt=time(NULL);
    srand(lt);
    
    int i,j,index1,index2;
    int no_of_bins=12;                           //no of bins for the histogram

  // CvCapture* vid=cvCreateFileCapture("video1.AVI");
   CvCapture* vid=cvCreateCameraCapture(0);
    cvGrabFrame(vid);
    IplImage* src=cvRetrieveFrame(vid);
    IplImage* src2;
    cvNamedWindow("Source",1);
   
      //declaring myu's alpha's and sigma's
   u=new CvScalar[k];
   sigma=new int[k];
   alpha=new double[k];

 
    start:
    p=0;
    x_centroid=0;
    y_centroid=0;
    no_of_pixels=0;
    x_centroid_temp=0,y_centroid_temp=0;
    x_prev_centroid = 0; y_centroid = 0; 
    no_of_pixels_temp=0;
    top_c=0,down_c=0,left_c=0,right_c=0; 

    
    while(1)
    {
        cvShowImage("Source",src);
        if(cvWaitKey(15)==32)
        {
           src2=cvCloneImage(src);
           break;                 
        }
        cvGrabFrame(vid);
        src=cvRetrieveFrame(vid);
    }
    cvReleaseImage(&src);
    src=cvCloneImage(src2);
    cvSetMouseCallback("Source",get_coordinate,src);
    cvWaitKey(0);

    //to keep the range of boundary points in limit
    if(x_3<0)
    x_3=0;
    if(x_4>=src->height)
    x_4=src->height-1;
    if(y_3<0)
    y_3=0;
    if(y_4>=src->width)
    y_4=src->width-1;
   
   //assigning values to myu's alpha's and sigma's
   for(i=0;i<k;i++)
   {
      u[i]=cvScalar(rand()%256,rand()%256,rand()%256);
      sigma[i]=rand()%250;   
   }   
   
   //to seperate the object;
    cvSetImageROI(src,cvRect(y_1,x_1,y_2-y_1+1,x_2-x_1+1));
    cvSaveImage("obj_img.jpg",src);
    IplImage* obj=cvLoadImage("obj_img.jpg");
    cvResetImageROI(src);

//    cvNamedWindow("Object",1);
//    cvShowImage("Object",obj);
    
    
    //background seperation 
    for(i=x_1;i<=x_2;i++)
     for(j=y_1;j<=y_2;j++)
     cvSet2D(src,i,j,cvScalar(0,0,0));                
    
    cvSetImageROI(src,cvRect(y_3,x_3,y_4-y_3+1,x_4-x_3+1));
    cvSaveImage("bg_img.jpg",src);
    IplImage* bg=cvLoadImage("bg_img.jpg");
//    cvNamedWindow("background",1);
//    cvShowImage("background",bg);
    cvResetImageROI(src); 
    
    ofstream fout("bg_values.txt");
    int count=0;
    
    for(i=x_3;i<=x_4;i++)
     for(j=y_3;j<=y_4;j++)
      {
           if(i>=x_1 && i<=x_2 && j>=y_1 && j<=y_2)
           {
            }
           else
           {
               count++;               
               CvScalar s=cvGetColor(src,i,j);
               fout<<s.val[0]<<" "<<s.val[1]<<" "<<s.val[2]<<endl;
               }                                       
      }    
    
    fout.close();  


    //finding histogram for object 
    int obj_bgr[no_of_bins][no_of_bins][no_of_bins];
    for(int i=0;i<no_of_bins;i++)
     for(int j=0;j<no_of_bins;j++)
      for(int k=0;k<no_of_bins;k++)
       obj_bgr[i][j][k]=0;
    
    double divide_no=255/no_of_bins*1.0;                //to find the bin number

    for(i=0;i<obj->height;i++)
     for(j=0;j<obj->width;j++)
     {
         CvScalar color=cvGetColor(obj,i,j);                         
         int bcolor,gcolor,rcolor;
         bcolor=color.val[0];
         gcolor=color.val[1];
         rcolor=color.val[2];
                           
         int bindex,gindex,rindex;
         bindex=(int)bcolor/divide_no;
         gindex=(int)gcolor/divide_no;
         rindex=(int)rcolor/divide_no;
         
         if(bindex==no_of_bins)
            bindex=no_of_bins-1;
         if(gindex==no_of_bins)
            gindex=no_of_bins-1;
         if(rindex==no_of_bins)
            rindex=no_of_bins-1;

         obj_bgr[bindex][gindex][rindex]++;                     
     }



    //finding histogram for background
    ifstream fin("bg_values.txt"); 
    int bg_bgr[no_of_bins][no_of_bins][no_of_bins];     
    
    for(int i=0;i<no_of_bins;i++)
     for(int j=0;j<no_of_bins;j++)
      for(int k=0;k<no_of_bins;k++)
       bg_bgr[i][j][k]=0;

    
    while(!fin.eof())
     {
         int bcolor,gcolor,rcolor;
         fin>>bcolor>>gcolor>>rcolor; 
         
         int bindex,gindex,rindex;
         bindex=(int)bcolor/divide_no;
         gindex=(int)gcolor/divide_no;
         rindex=(int)rcolor/divide_no;
         
         
         if(bindex==no_of_bins)
            bindex=no_of_bins-1;
         if(gindex==no_of_bins)
            gindex=no_of_bins-1;
         if(rindex==no_of_bins)
            rindex=no_of_bins-1;
            
         bg_bgr[bindex][gindex][rindex]++;                     
     }
      
   
   //binary image - likelihood ratio and then thresholding
   IplImage* binary_image=cvCreateImage(cvGetSize(src),8,1);
   cvZero(binary_image);
   
   for(i=x_3;i<=x_4;i++)
   { 
        for(j=y_3;j<=y_4;j++)
        { 
           CvScalar color=cvGetColor(src2,i,j);
           int bcolor,gcolor,rcolor;
           bcolor=color.val[0]; 
           gcolor=color.val[1];
           rcolor=color.val[2];
           
           int bindex,gindex,rindex;
           bindex=(int)bcolor/divide_no;
           gindex=(int)gcolor/divide_no;
           rindex=(int)rcolor/divide_no;
                  
           if(bindex==no_of_bins)
              bindex=no_of_bins-1;
           if(gindex==no_of_bins)
              gindex=no_of_bins-1;
           if(rindex==no_of_bins)
              rindex=no_of_bins-1;
         
           double obj_prob,bg_prob,value,thresh,thr;
           obj_prob=obj_bgr[bindex][gindex][rindex]*1.0/((x_2-x_1+1)*(y_2-y_1+1));
         
           bg_prob=bg_bgr[bindex][gindex][rindex]*1.0/count;                
        
           thr=0.001;
           if(bg_prob<=thr)
              bg_prob=thr;
         
           if(obj_prob<=thr)
              obj_prob=thr;
        
           value=log(obj_prob/bg_prob);

           thresh=0.55;
           
           if(value>=thresh)
           {
               cvSetColor(binary_image,i,j,cvRealScalar(255));
			   original_count++;
           }
           else                        
             cvSetColor(binary_image,i,j,cvRealScalar(0));
        }          
   }
   cvNamedWindow("Binary Image");
   cvShowImage("Binary Image",binary_image);
  cvWaitKey(0); 
  cvDestroyWindow("Binary Image"); 
   
   //inputting into the matrix to find pseudoinverse
   unsigned long cnt=(x_2-x_1+1)*(y_2-y_1+1);
   CvMat* mat=cvCreateMat(cnt,k,CV_32FC1);
   CvMat* inv_mat=cvCreateMat(k,cnt,CV_32FC1);
   unsigned long counter=0;  
   Y=new double[cnt];

   for(i=x_1;i<=x_2;i++)
    {
      for(j=y_1;j<=y_2;j++)
       {      
               CvScalar s1=cvGetColor(binary_image,i,j);
               CvScalar s2=cvGetColor(src2,i,j);
               if(s1.val[0]==255.0)
               { Y[counter]=1; }
               else
               { Y[counter]=-1; }
               for(int tp=0;tp<k;tp++)
               {
                   cvmSet(mat,counter,tp,exp(-1*diff(s2,u[tp])/sigma[tp]));
               }
               counter++;          
       }
    }
      
   cvPseudoInv(mat,inv_mat);  



   //calculating alpha values - that is weight of the neurons
   ofstream fut("result.txt");    
         
   for(i=0;i<k;i++)
   {
       double sum=0;                         
       
       for(j=0;j<counter;j++)  
       {
            sum+=(cvmGet(inv_mat,i,j)) * Y[j];             
       }
       
       alpha[i]=sum;
       fut<<sum<<"\n";     
   }
fut.close();
   cout<<"\n Phase I completed\n";
  // cvWaitKey(0);
    
   cvReleaseImage(&src);
   src=cvCloneImage(src2);
    
    for(i=0;i<src->height;i++)
    {
        for(j=0;j<src->width;j++)
        {
              CvScalar color1=cvGetColor(src2,i,j);
               float ans=0;
               
               for(int tp=0;tp<k;tp++)
               {
                      ans+=exp(-1*diff(color1,u[tp])/sigma[tp]) * alpha[tp];
               }
               
               if(ans>=0.5 && ans <=1.5)
                {  
                    cvSetColor(src2,i,j,cvScalarAll(255));
                    if(i>=x_1 && i<=x_2 && j>=y_1 && j<=y_2)
                    {
                        x_centroid+=i;
                        y_centroid+=j;
                        no_of_pixels++;
                    }
                }                     
        }                          
    }
    
    
   // calculation of Centroid
    x_centroid=x_centroid/no_of_pixels*1.0;
    y_centroid=y_centroid/no_of_pixels*1.0;
//    cout<<x_centroid<<"  "<<y_centroid;
    
    top_c=x_centroid-x_1;
    down_c=x_2-x_centroid;
    left_c=y_centroid-y_1;
    right_c=y_2-y_centroid;
    
    

  //  cvNamedWindow("frame1");
   // cvShowImage("frame1",src2);
    
  //  cvWaitKey(0);
    
    
    
    //Starting Object Tracking
    cout<<"\n Started Tracking\n";
    while(cvGrabFrame(vid))
    {                
	x_prev_centroid = x_centroid;
	y_prev_centroid = y_centroid;	

       IplImage* frame_2=cvRetrieveFrame(vid);
       IplImage* binary=cvCloneImage(binary_image);
       cvZero(binary);
    
       CvScalar color1;
      
	   temp_count = 0;
    //classifying into object and non-object using RBF Classifier
    
       for(i=x_1;i<=x_2;i++)
       {
           for(j=y_1;j<=y_2;j++)
            {
                 color1=cvGetColor(frame_2,i,j);
                 float ans=0;
                 for(int tp=0;tp<k;tp++)
                  {
                      ans+=exp(-1*diff(color1,u[tp])/sigma[tp]) * alpha[tp];
                  }
                 
                 if(ans>=0.75 && ans <=2.0)
                  {  
                      cvSetColor(binary,i,j,cvRealScalar(255));
					  temp_count++;
                  }                     
            }                          
        }

    cvNamedWindow("frame2");
   cvShowImage("frame2",binary);
    
    
       int run=1;
       while(run<50)
       {         
           y_centroid_temp=0;
           x_centroid_temp=0;
           no_of_pixels_temp=0;

		   temp_count = 0;
           for(i=x_1;i<=x_2;i++)
           {
               for(j=y_1;j<=y_2;j++)
               {
                  if(cvGetColor(binary,i,j).val[0]==255)
                  {
                     x_centroid_temp+=i;
                     y_centroid_temp+=j;                     
                     no_of_pixels_temp++;      
                  }
               }                    
           }

           x_centroid_temp/=no_of_pixels_temp*1.0; 
           y_centroid_temp/=no_of_pixels_temp*1.0; 

           if(cvCalcDist(x_centroid,y_centroid,x_centroid_temp,y_centroid_temp)<=5 || run==45)
           {                                                                          
              x_centroid=x_centroid_temp;   
              y_centroid=y_centroid_temp;
             
              x_1=x_centroid - top_c;
              x_2=x_centroid + down_c;
              y_1=y_centroid - left_c;
              y_2=y_centroid + right_c;
           
              if(x_1<0)
              { x_1=0;   }
        
              if(x_2>=src->height)
              { x_2=src->height - 1;  }
        
              if(y_1<0)
              {  y_1=0;   }
        
              if(y_2>=src->width)
              {   y_2=src->width - 1;   }
         
            //drawing the rectangle to enclose object    
              cvRectangle(frame_2,cvPoint(y_1,x_1),cvPoint(y_2,x_2),cvScalarAll(255),1);
              break;
            }
    
            else
            {
              x_centroid=x_centroid_temp;   
              y_centroid=y_centroid_temp;

              x_1=x_centroid - top_c;
              x_2=x_centroid + down_c;
              y_1=y_centroid - left_c;
              y_2=y_centroid + right_c;
            
              if(x_1<0)
              { x_1=0;   }
        
              if(x_2>=src->height)
              { x_2=src->height - 1;  }
        
              if(y_1<0)
              {  y_1=0;   }
         
              if(y_2>=src->width)
              {   y_2=src->width - 1;   }
             }
        
            run++;
    }                                            //end of while loop
    

    cvNamedWindow("video");
    cvShowImage("video",frame_2);
    
	
	/* Guiding the robot
	float percent_change = 0;
	percent_change = (no_of_pixels - no_of_pixels_temp)/no_of_pixels;
	//based on the percent_change value speed of the robot is controlled
	//based on the centroid change turning parameters are calculated
	int x_chng_centroid=0,y_chng_centroid=0;
	x_chng_centroid = x_centroid - x_prev_centroid;
	y_chng_centroid = y_centroid - y_prev_centroid;
	
	int theta=0;
	theta = arctan(y/l_approx);
	
	//code for rotating by theta radians - this is for orientation - to keep object localizes to the center even if the target is taking a turn
	
	//code for speeding up - this is for maintaining a constant distance with the object
	if(percent_change > 0)
	{
		speedup
	}
	
	else{
		slowdown 
	}
	
	usleep(10);

		
	
	
	*/
	
	
    int FLAG=0;
    FLAG=cvWaitKey(1);
    
    if(FLAG==32)
    cvWaitKey(0);
    
    if(FLAG==27)
    goto start;

}

    return 0; 
}

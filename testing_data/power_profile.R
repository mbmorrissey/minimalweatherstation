## read in data from ppk2

setwd("/users/michaelmorrissey/Documents/repositories")
setwd("./minimalweatherstation/testing_data")

d<-read.table("./power_profile_raw.csv",header=TRUE,sep=',')

## main plot of one minute of data

png("./minimalweatherpower1.png",height=480,width=620)
plot(1:(dim(d)[1])/1000,d[,2]/1000,type='l',
   xlab="time (s)", ylab="current (mA)")
dev.off()


png("./minimalweatherpower2.png",height=480,width=620)
range<-1:5000
plot(range,d[range,2],type='l',
   xlab="time (ms)", ylab="current (uA)")
dev.off()





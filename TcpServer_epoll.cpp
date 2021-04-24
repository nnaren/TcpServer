#include <sys/epoll.h>
int AcceptByEpoll(int serverFd)
{
   int efd = epoll_create1( 0 );
   if ( efd == - 1 )
     {
       perror ( & quot ; epoll_create & quot ; ) ;
       abort ( ) ;
     }
 
   event . data . fd = sfd ;
   event . events = EPOLLIN | EPOLLET ;
   s = epoll_ctl ( efd , EPOLL_CTL_ADD , sfd , & amp ; event ) ;
   if ( s == - 1 )
     {
       perror ( & quot ; epoll_ctl & quot ; ) ;
       abort ( ) ;
     }
 
   /* Buffer where events are returned */
   events = calloc ( MAXEVENTS , sizeof event ) ;
 
   /* The event loop */
   while ( 1 )
     {
       int n , i ;
 
       n = epoll_wait ( efd , events , MAXEVENTS , - 1 ) ;
       for ( i = 0 ; i & lt ; n ; i ++ )
     {
       if ( ( events [ i ] . events & amp ; EPOLLERR ) ||
               ( events [ i ] . events & amp ; EPOLLHUP ) ||
               ( ! ( events [ i ] . events & amp ; EPOLLIN ) ) )
         {
               /* An error has occured on this fd, or the socket is not
                 ready for reading (why were we notified then?) */
           fprintf ( stderr , & quot ; epoll errorn & quot ; ) ;
           close ( events [ i ] . data . fd ) ;
           continue ;
         }
 
       else if ( sfd == events [ i ] . data . fd )
         {
               /* We have a notification on the listening socket, which
                 means one or more incoming connections. */
               while ( 1 )
                 {
                   struct sockaddr in_addr ;
                   socklen_t in_len ;
                   int infd ;
                   char hbuf [ NI_MAXHOST ] , sbuf [ NI_MAXSERV ] ;
 
                   in_len = sizeof in_addr ;
                   infd = accept ( sfd , & amp ; in_addr , & amp ; in_len ) ;
                   if ( infd == - 1 )
                     {
                       if ( ( errno == EAGAIN ) ||
                           ( errno == EWOULDBLOCK ) )
                         {
                           /* We have processed all incoming
                             connections. */
                           break ;
                         }
                       else
                         {
                           perror ( & quot ; accept & quot ; ) ;
                           break ;
                         }
                     }
 
                   s = getnameinfo ( & amp ; in_addr , in_len ,
                                   hbuf , sizeof hbuf ,
                                   sbuf , sizeof sbuf ,
                                   NI_NUMERICHOST | NI_NUMERICSERV ) ;
                   if ( s == 0 )
                     {
                       printf ( & quot ; Accepted connection on descriptor % d & quot ;
                             & quot ; ( host = % s , port = % s ) n & quot ; , infd , hbuf , sbuf ) ;
                     }
 
                   /* Make the incoming socket non-blocking and add it to the
                     list of fds to monitor. */
                   s = make_socket_non_blocking ( infd ) ;
                   if ( s == - 1 )
                     abort ( ) ;
 
                   event . data . fd = infd ;
                   event . events = EPOLLIN | EPOLLET ;
                   s = epoll_ctl ( efd , EPOLL_CTL_ADD , infd , & amp ; event ) ;
                   if ( s == - 1 )
                     {
                       perror ( & quot ; epoll_ctl & quot ; ) ;
                       abort ( ) ;
                     }
                 }
               continue ;
             }
           else
             {
               /* We have data on the fd waiting to be read. Read and
                 display it. We must read whatever data is available
                 completely, as we are running in edge-triggered mode
                 and won&#039;t get a notification again for the same
                 data. */
               int done = 0 ;
 
               while ( 1 )
                 {
                   ssize_t count ;
                   char buf [ 512 ] ;
 
                   count = read ( events [ i ] . data . fd , buf , sizeof buf ) ;
                   if ( count == - 1 )
                     {
                       /* If errno == EAGAIN, that means we have read all
                         data. So go back to the main loop. */
                       if ( errno != EAGAIN )
                         {
                           perror ( & quot ; read & quot ; ) ;
                           done = 1 ;
                         }
                       break ;
                     }
                   else if ( count == 0 )
                     {
                       /* End of file. The remote has closed the
                         connection. */
                       done = 1 ;
                       break ;
                     }
 
                   /* Write the buffer to standard output */
                   s = write ( 1 , buf , count ) ;
                   if ( s == - 1 )
                     {
                       perror ( & quot ; write & quot ; ) ;
                       abort ( ) ;
                     }
                 }
 
               if ( done )
                 {
                   printf ( & quot ; Closed connection on descriptor % dn & quot ; ,
                           events [ i ] . data . fd ) ;
 
                   /* Closing the descriptor will make epoll remove it
                     from the set of descriptors which are monitored. */
                   close ( events [ i ] . data . fd ) ;
                 }
             }
         }
     }
 
   free ( events ) ;
 
   close ( sfd ) ;
 
   return EXIT_SUCCESS ;
}
﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

namespace CollectorsFrontEnd
{
   class ServerIFace
   {
      List<string> GetCollection(string aszCollectionName)
      {
         List<string> lstRetVal = new List<string>();

         return lstRetVal;
      }

      void ServerSocket()
      {
         byte[] bytes = new byte[1024];

         // Connect to a remote device.
         try
         {
            // Establish the remote endpoint for the socket.
            // This example uses port 11000 on the local computer.
            IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            IPAddress ipAddress = ipHostInfo.AddressList[0];
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, 11000);

            // Create a TCP/IP  socket.
            Socket sender = new Socket(AddressFamily.InterNetwork,
                SocketType.Stream, ProtocolType.Tcp);

            // Connect the socket to the remote endpoint. Catch any errors.
            try
            {
               sender.Connect(remoteEP);

               Console.WriteLine("Socket connected to {0}",
                   sender.RemoteEndPoint.ToString());

               // Encode the data string into a byte array.
               byte[] msg = Encoding.ASCII.GetBytes("This is a test<EOF>");

               // Send the data through the socket.
               int bytesSent = sender.Send(msg);

               // Receive the response from the remote device.
               int bytesRec = sender.Receive(bytes);
               Console.WriteLine("Echoed test = {0}",
                   Encoding.ASCII.GetString(bytes, 0, bytesRec));

               // Release the socket.
               sender.Shutdown(SocketShutdown.Both);
               sender.Close();

            }
            catch (ArgumentNullException ane)
            {
               Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
            }
            catch (SocketException se)
            {
               Console.WriteLine("SocketException : {0}", se.ToString());
            }
            catch (Exception e)
            {
               Console.WriteLine("Unexpected exception : {0}", e.ToString());
            }

         }
         catch (Exception e)
         {
            Console.WriteLine(e.ToString());
         }
      }
   }


}

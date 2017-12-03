using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   interface IServerObserver
   {
      void ServerMessage(string Message);
   }
}

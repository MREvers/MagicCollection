using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   interface IModel
   {
      void Register(IViewModel item);
      void UnRegister(IViewModel item);
      void NotifyViewModel();

      void EnableNotification(bool abNotify);
      void DisableNotification();

      // Not every implementation is required to implement a Synchronous version.
      void Sync(bool ASync = true);
   }
}

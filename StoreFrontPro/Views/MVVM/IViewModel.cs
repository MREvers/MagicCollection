using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   interface IViewModel
   {
      void ModelUpdated();
      void FreeModel();
   }
}

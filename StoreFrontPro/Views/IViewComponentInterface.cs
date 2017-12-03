using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   /// <summary>
   /// Any class that wants to support a sub component needs to implement
   /// that components "ViewComponentInterface". Also, that class must also
   /// have a corresponding ComponentInterface.
   /// 
   /// It is recommended that the constructor of each of these takes
   /// Func<object,Action> so that a static instance of the interface
   /// may be constructed, and type checking is still enforced.
   /// 
   /// Eg
   /// public IVCI((<ConcreteIVCI>x)=>{return x.method;})
   /// </summary>
    interface IViewComponentInterface
    {
        Type GetInterfaceType();
        bool TryInvoke(object Caller, string Key, object[] args);
    }
}

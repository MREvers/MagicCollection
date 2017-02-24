using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd
{
    public delegate void ComponentEvent(IDataModel aDataObject, string szAction);
    public interface IComponent
    {
        event ComponentEvent UnhandledEvent;
        void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction);
        IDataModel GetDataModel();
    }
}

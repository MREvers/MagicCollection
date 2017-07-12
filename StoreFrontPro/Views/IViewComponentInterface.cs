using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
    interface IViewComponentInterface
    {
        Type GetInterfaceType();
        bool TryInvoke(string Key, object[] args);
    }
}

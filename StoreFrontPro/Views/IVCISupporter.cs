using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
    interface IVCISupporter
    {
        Dictionary<Type, IViewComponentInterface> SupportedInterface { get; set; }
        void DisplayEventHandler(object source, DisplayEventArgs e);
    }
}

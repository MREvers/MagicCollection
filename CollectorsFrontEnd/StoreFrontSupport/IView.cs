using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    #region Enumerations

    public enum ViewComponentHandles
    {
        MenuItem,
        Buttons
    }

    #endregion

    interface IView
    {

        #region Interface

        // The calling object will know to which type to cast the 'object'.
        List<object> GetViewHook(ViewComponentHandles ViewHandle);

        #endregion
    }
}

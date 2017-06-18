using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    #region Enumerations

    public enum ViewComponentHandles
    {
        MenuItem,
        Buttons
    }

    #endregion

    #region Events & Delegates

    public delegate void DisplayEventHandler(string Event, UserControl Source);

    #endregion

    interface IView
    {
        #region Interface

        // Fires whenever the VC cannot handle a UI Event.
        /// <summary>
        /// Views should fire the event as "<ComponentName>.<VarName>.<Event>"
        /// </summary>
        event DisplayEventHandler DisplayEvent;

        // Contains things like menu-items and button events not handled by the VC.
        List<object> GetViewHook(ViewComponentHandles ViewHandle);

        #endregion
    }
}

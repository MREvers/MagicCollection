using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Components.PlusMinusControl
{
   class MPlusMinusControl : IModel
   {
      private int _Value = 0;
      public int Value
      {
         get { return _Value; }
         set { _Value = Math.Max(1, Math.Min(m_iMax, value)); }
      }

      private int m_iMax;

      public MPlusMinusControl(int DefaultInt = 1, int Max = 99)
      {
         Value = DefaultInt;
         m_iMax = Max;
      }

      #region IModel
      public void Register(IViewModel item)
      {
         throw new NotImplementedException();
      }

      public void UnRegister(IViewModel item)
      {

      }

      public void NotifyViewModel()
      {
         throw new NotImplementedException();
      }

      public void EnableNotification(bool abNotify)
      {
         throw new NotImplementedException();
      }

      public void DisableNotification()
      {
         throw new NotImplementedException();
      }

      public void Sync(bool ASync = true)
      {
         throw new NotImplementedException();
      }
      #endregion
   }
}

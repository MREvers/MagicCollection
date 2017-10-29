using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
   class VCIFancyCollectionList: IViewComponentInterface
   {
      public const string ChildClicked = "Clicked";
      public const string ListChanged = "Change";

      private Func<object, Action<CardModel>> m_Clicked;
      private Func<object, Action> m_ListChanged;

      public VCIFancyCollectionList(Func<object, Action<CardModel>> Clicked,
                                    Func<object, Action> ListUpdated)
      {
         m_Clicked = Clicked;
         m_ListChanged = ListUpdated;
      }

      public Type GetInterfaceType()
      {
         return typeof(VMFancyCollectionList);
      }

      public bool TryInvoke(object Caller, string Key, object[] args)
      {
         if (Key == ChildClicked)
         {
            if (args?[0] is CardModel)
            {
               CardModel paramOne = (CardModel) args[0];
               m_Clicked?.Invoke(Caller).Invoke(paramOne);
               return true;
            }
         }
         else if( Key == ListChanged )
         {
            m_ListChanged?.Invoke(Caller).Invoke();
            return true;
         }

         return false;
      }
   }
}

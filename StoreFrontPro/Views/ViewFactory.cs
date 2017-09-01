using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.SuggestionsSearchBox;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
   static class ViewFactory
   {
      public static ViewClass CreateCollectionEditorItem(CollectionDelta Delta, string RoutingName)
      {
         MCollectionEditorItem ceiM = new MCollectionEditorItem(Delta);
         VMCollectionEditorItem ceiVM = new VMCollectionEditorItem(ceiM, RoutingName);
         VCollectionEditorItem ceiV = new VCollectionEditorItem() { DataContext = ceiVM };

         return new ViewClass(ceiM, ceiVM, ceiV);
      }

      public static ViewClass CreateSuggestionSearchBox( Func<string, List<string>> SearchCollection, 
                                                         string ActionName, string RoutingName )
      {
         MSuggestionsSearchBox ssBoxM = new MSuggestionsSearchBox(
             ActionName: ActionName,
             SearchCollection: SearchCollection);
         VMSuggestionsSearchBox ssBoxVM = new VMSuggestionsSearchBox(ssBoxM, RoutingName);
         VSuggestionsSearchBox ssBoxV = new VSuggestionsSearchBox() { DataContext = ssBoxVM };

         return new ViewClass(ssBoxM, ssBoxVM, ssBoxV);
      }
   }
}

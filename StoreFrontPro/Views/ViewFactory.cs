using StoreFrontPro.Server;
using StoreFrontPro.Views.CollectionViews.Cube;
using StoreFrontPro.Views.Components.CardImageDisplayer;
using StoreFrontPro.Views.Components.VCardImageDock;
using StoreFrontPro.Views.Components.SuggestionsSearchBox;
using StoreFrontPro.Views.Interfaces.CollectionChanger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using StoreFrontPro.Views.CollectionViews.Deckbox;
using StoreFrontPro.Views.Interfaces.CardInterface;
using StoreFrontPro.Views.Components.CopySelector;

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

      public static ViewClass CreateSuggestionSearchBox(Func<string, List<string>> SearchCollection,
                                                         string ActionName, string RoutingName)
      {
         MSuggestionsSearchBox ssBoxM = new MSuggestionsSearchBox(
             ActionName: ActionName,
             SearchCollection: SearchCollection);
         VMSuggestionsSearchBox ssBoxVM = new VMSuggestionsSearchBox(ssBoxM, RoutingName);
         VSuggestionsSearchBox ssBoxV = new VSuggestionsSearchBox() { DataContext = ssBoxVM };

         return new ViewClass(ssBoxM, ssBoxVM, ssBoxV);
      }

      public static ViewClass CreateCardGroupList(string FilteredAttribute, string ExpectedAttribute,
                                                   List<string> AttributeSet, BasicModel<List<CardModel>> BaseModel)
      {
         MCardGroupList oCardGroupList =
            new MCardGroupList(FilteredAttribute, ExpectedAttribute, AttributeSet, BaseModel);

         if (oCardGroupList.GroupedList.Count > 0)
         {
            VMCardGroupList oCardGroupListVM =
               new VMCardGroupList(oCardGroupList, "", AttributeSet.Contains(ExpectedAttribute));

            VCardGroupList oCardGroupListV =
               new VCardGroupList() { DataContext = oCardGroupListVM };

            return new ViewClass(oCardGroupList, oCardGroupListVM, oCardGroupListV);
         }

         return null;
      }

      public static ViewClass CreateFancyCollectionItem(CardModel Model, int Columns, string RoutingName)
      {
         VMFancyCollectionItem fancyCollectionItemVM = new VMFancyCollectionItem(Model, RoutingName, Columns);
         VFancyCollectionItem fancyCollectionItemV = new VFancyCollectionItem() { DataContext = fancyCollectionItemVM };

         return new ViewClass(Model, fancyCollectionItemVM, fancyCollectionItemV);
      }

      public static ViewClass CreateCardImageViewer(CardModel Model, string RoutingName)
      {
         VMCardImageDisplayer cardImageDisplayerVM = new VMCardImageDisplayer(Model, RoutingName);
         VCardImageDisplayer cardImageDisplayerV = new VCardImageDisplayer() { DataContext = cardImageDisplayerVM };

         return new ViewClass(Model, cardImageDisplayerVM, cardImageDisplayerV);
      }

      public static ViewClass CreateItemDock(string RoutingName)
      {
         MCardImageDock cardImageDockM = new MCardImageDock();
         VMCardImageDock cardImageDockVM = new VMCardImageDock(cardImageDockM, RoutingName);
         VCardImageDock cardImageDockV = new VCardImageDock() { DataContext = cardImageDockVM };

         return new ViewClass(cardImageDockM, cardImageDockVM, cardImageDockV);
      }

      public static ViewClass CreateAttributeEditorItem(string TraitName, List<string> Options, string RoutingName)
      {
         MAttributeEditorItem attributeEditorItemM = new MAttributeEditorItem(TraitName, Options);
         VMAttributeEditorItem attributeEditorItemVM = new VMAttributeEditorItem(attributeEditorItemM, RoutingName);
         VAttributeEditorItem attributeEditorItemV = new VAttributeEditorItem() { DataContext = attributeEditorItemVM };
         
         return new ViewClass(attributeEditorItemM, attributeEditorItemVM, attributeEditorItemV);
      }

      public static ViewClass CreateCopySelector(CardModel Model, string RoutingName)
      {
         VMCopySelector selectorVM = new VMCopySelector(Model, RoutingName);
         VCopySelector selectorV = new VCopySelector() { DataContext = selectorVM };

         return new ViewClass(Model, selectorVM, selectorV);
      }

      public static ViewClass CreateCopySelectorCopyItem(string Name, string RoutingName)
      {
         BasicModel<string> copyM = new BasicModel<string>(Name, null);
         VMCopy copyVM = new VMCopy(copyM, RoutingName);
         VCopy copyV =new VCopy() { DataContext = copyVM };

         return new ViewClass(copyM, copyVM, copyV);
      }
   }
}

using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views.Interfaces.CardInterface
{
   class VMCardChanger : ViewModel<CardModel>, IVCISupporter, IViewComponent
   {
      #region Bindings
      public ObservableCollection<VAttributeEditorItem> IdentifyingAttributes { get; set; }
         = new ObservableCollection<VAttributeEditorItem>();
      #endregion

      public VMCardChanger(CardModel Model, string RoutingName)
         : base(Model, RoutingName)
      {
      }

      public override void ModelUpdated()
      {
         throw new NotImplementedException();
      }

      public void SetEditting(CardModel Model)
      {
         this.Model = Model;
         setIdentifyingTraitViews();
      }

      private void setIdentifyingTraitViews()
      {
         clearIdentifyingTraits();

         CardModelBase oProto = CardModel.GetPrototype(Model.PrototypeName);
         foreach( var trait in oProto.AttributeOptions )
         {
            addIdentifyingTrait(trait);
         }
      }

      private void addIdentifyingTrait(KeyValuePair<string,List<string>> trait)
      {
         ViewClass traitView = ViewFactory.CreateAttributeEditorItem(trait.Key, trait.Value, trait.Key);
         IdentifyingAttributes.Add((VAttributeEditorItem) traitView.View);

         // Set the trait to the current one.
         string szCurrentTrait = Model.GetAttr(trait.Key);
         ((VMAttributeEditorItem) traitView.ViewModel).Set = szCurrentTrait;
         ((VMAttributeEditorItem) traitView.ViewModel).DisplayEvent += DisplayEventHandler;
      }

      private void clearIdentifyingTraits()
      {
         foreach( var viewTrait in IdentifyingAttributes )
         {
            ((VMAttributeEditorItem) viewTrait.DataContext).DisplayEvent -= DisplayEventHandler;
         }
         IdentifyingAttributes.Clear();
      }

      private VMAttributeEditorItem editorItem(VAttributeEditorItem item)
      {
         return (VMAttributeEditorItem) item.DataContext;
      }

      #region Event Handlers
      private void identifierChanged(string aszRouting, string aszNewVal)
      {
         CardModelBase oProto = CardModel.GetPrototype(Model.PrototypeName);

         // Find the index of the newly selected value.
         var lstOptions = oProto.AttributeOptions[aszRouting];
         int index = lstOptions.FindIndex(a => a == aszNewVal);

         // Update any paired traits
         foreach( var pairedTrait in ServerInterface.Card.GetPairedAttributes() )
         {
            string szSearch = "";
            if( pairedTrait.Item1 == aszRouting )
            {
               szSearch = pairedTrait.Item2;
            }
            else if ( pairedTrait.Item2 == aszRouting )
            {
               szSearch = pairedTrait.Item1;
            }

            if( szSearch != "" )
            {
               var otherPairItem = editorItem(
                  IdentifyingAttributes.FirstOrDefault
                  (x => editorItem(x).AttributeName == szSearch 
                  ));

               if( otherPairItem != null )
               {
                  otherPairItem.Model.DisableNotification();
                  otherPairItem.Set = otherPairItem.Model.Options[index];
                  otherPairItem.Model.EnableNotification();
               }
            }
         }

         Model.Set
      }
      #endregion

      #region IViewComponent
      public void DisplayEventHandler(object source, DisplayEventArgs e)
      {
         GetRouter().Call(source.GetType(), this, e.Key, e.Args);
      }

      public List<StoreFrontMenuItem> GetMenuItems()
      {
         throw new NotImplementedException();
      }
      #endregion

      #region IVCISupporter
      public event DisplayEventHandler DisplayEvent;

      public InterfaceRouter GetRouter()
      {
         return IRouter;
      }

      static InterfaceRouter _IRouter = null;
      static InterfaceRouter IRouter
      {
         get
         {
            if (_IRouter == null) { BuildInterface(); }
            return _IRouter;
         }
      }

      static void BuildInterface()
      {
         _IRouter = new InterfaceRouter();

         // TODO: the routing is important here because there is more than one.
         VCIAttributeEditorItem VCAddIS = new VCIAttributeEditorItem(
            SelectionChanged: (x) => { return (x as VMCardChanger).identifierChanged; });

         _IRouter.AddInterface(VCAddIS);
      }
      #endregion
   }
}

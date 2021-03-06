﻿using StoreFrontPro.Server;
using StoreFrontPro.Views.Components.ManaViewer;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace StoreFrontPro.Views.CollectionViews.Deckbox
{
   class VMFancyCollectionItem : ViewModel<CardModel>, IViewComponent
   {
      public List<string> LST_TEMP_IMPORTANT_ATTRS = new List<string>()
        {
            "manaCost",
            "set"
        };

      public int Columns { get; set; } = 3;

      public RelayCommand MouseOver { get; set; }
      public ObservableCollection<UIElement> DisplayedProperties { get; set; } =
         new ObservableCollection<UIElement>();

      public VMFancyCollectionItem(CardModel Model, string RoutingName, int Columns = 3) 
         : base(Model, RoutingName)
      {
         this.Columns = Columns;
         MouseOver = new RelayCommand(eClicked);
         Model.Register(this);
         SyncWithModel();
      }

      public void SyncWithModel()
      {
         DisplayedProperties.Clear();

         if (Columns != 1)
         {
            DisplayedProperties.Add(new TextBox() { Text = "x" + Model.Count, IsReadOnly = true });
            DisplayedProperties.Add(new TextBox() { Text = Model.DisplayName, IsReadOnly = true });
            foreach (string szKey in LST_TEMP_IMPORTANT_ATTRS)
            {
               if (szKey == "manaCost")
               {
                  VMManaViewer manaViewerVM = new VMManaViewer(Model.GetAttr(szKey), "");
                  VManaViewer manaViewerV = new VManaViewer();
                  manaViewerV.DataContext = manaViewerVM;
                  DisplayedProperties.Add(manaViewerV);
               }
               else
               {
                  DisplayedProperties.Add(new TextBox() { Text = Model.GetAttr(szKey), IsReadOnly = true });
               }
            }
         }
         else
         {
            DisplayedProperties.Add(new TextBox() { Text = Model.DisplayName, IsReadOnly = true });
         }
      }

      private void eClicked(object canExecute)
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCIFancyCollectionItem.Clicked, Model);
         DisplayEvent?.Invoke(this, eventArgs);
      }

      #region IViewComponent
      public event DisplayEventHandler DisplayEvent;

      public List<StoreFrontMenuItem> GetMenuItems()
      {
         throw new NotImplementedException();
      }
      #endregion

      #region IViewModel
      public override void ModelUpdated()
      {
         SyncWithModel();
      }
      #endregion
   }
}

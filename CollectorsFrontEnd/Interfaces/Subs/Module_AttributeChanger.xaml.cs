﻿using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd.Interfaces.Subs
{
    /// <summary>
    /// Interaction logic for CompSubAttributeChanger.xaml
    /// </summary>
    public partial class Module_AttributeChanger : UserControl, IComponent, INotifyPropertyChanged
    {
        #region Data Binding

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion

        #region Sub Types
        public class MutableTuple
        {
            public MutableTuple(string aszFirst, string aszSecond)
            {
                First = aszFirst;
                Second = aszSecond;
            }
            public string First { get; set; }
            public string Second { get; set; }
        }

        public class MutableRestrictedList
        {
            public string Key { get; set; }
            public List<string> PossibleValues { get; set; }
            public string Value { get; set; }
        }

        public class CompSubAttributeChangerModel : IDataModel
        {
            public CompSubAttributeChangerModel(CardModel aDataModel, List<Tuple<string, string>> aLstCurrentTags)
            {
                CardModelObject = aDataModel;
                LstCurrentMetaTags = 
                    new ObservableCollection<MutableTuple>(aLstCurrentTags.Select(x => new MutableTuple(x.Item1, x.Item2)));
                    
                LstNonUniqueAttrs =
                    new ObservableCollection<MutableRestrictedList>();
                foreach (var aut in aDataModel.LstSpecifiedAttrs)
                {
                    List<string> lstRestrictedVals = aDataModel
                        .LstSpecifiedAttrsRestrictions
                        .Where(x => x.Item1 == aut.Item1)?.Select(x => x.Item2)?.FirstOrDefault();
                    if (lstRestrictedVals == null)
                    {
                        lstRestrictedVals = new List<string>();
                    }
                    SetNonUniqueAttribute(aut.Item1, aut.Item2, lstRestrictedVals);
                }
            }

            public void SetNonUniqueAttribute(string aszKey, string aszValue, List<string> alstPossibleVals)
            {
                MutableRestrictedList MRL = new MutableRestrictedList();
                MRL.Key = aszKey;
                MRL.Value = aszValue;
                MRL.PossibleValues = alstPossibleVals;

                LstNonUniqueAttrs.Add(MRL);
            }
            public CardModel CardModelObject;

            public ObservableCollection<MutableTuple> LstCurrentMetaTags { get; set; }
            public ObservableCollection<MutableRestrictedList> LstNonUniqueAttrs { get; set; }
        }
        #endregion

        #region Events
        public event ComponentEvent UnhandledEvent;
        #endregion Events

        #region Public Fields
        public CompSubAttributeChangerModel DataModel;
        #endregion Public Fields

        #region Private Fields
        private UserControl m_OverlayControl;
        #endregion

        #region Public Methods
        public Module_AttributeChanger(CardModel aDataModel)
        {
            InitializeComponent();

            DataModel = new CompSubAttributeChangerModel(aDataModel, aDataModel.LstMetaTags);
            DataContext = DataModel;
        }

        public IDataModel GetDataModel()
        {
            return DataModel;
        }

        #endregion Public Methods

        #region Private Methods
        private void showKeyValCreaterWindow()
        {
            showMainDisplay();

            Module_KeyValCreater ITI = new Module_KeyValCreater();
            m_OverlayControl = ITI;
            ITI.UnhandledEvent += RouteReceivedUnhandledEvent;
            Panel.SetZIndex(GrdOverlay, 2);
            GrdOverlay.Children.Add(ITI);
            GrdMain.IsEnabled = false;
        }


        private void showMainDisplay()
        {
            GrdOverlay.Children.Remove(m_OverlayControl);
            m_OverlayControl = null;
            GrdMain.IsEnabled = true;
        }
        #endregion

        #region Public Event Handlers
        public void RouteReceivedUnhandledEvent(IDataModel aDataObject, string aszAction)
        {
            if (aDataObject.GetType() == typeof(Module_KeyValCreater.Data))
            {
                Module_KeyValCreater.Data oDataModel =
                    (Module_KeyValCreater.Data)aDataObject;
                if (aszAction == "OK")
                {
                    ecAddMetaTag(oDataModel);
                }
                else if (aszAction == "Cancel")
                {
                    showMainDisplay();
                }
            }
        }
        #endregion Public EH

        #region Private Event Handlers
        private void ecAddMetaTag(Module_KeyValCreater.Data aDataObject)
        {
            if (aDataObject.Key != "")
            {
                DataModel.LstCurrentMetaTags.Add(new MutableTuple(aDataObject.Key, aDataObject.Value));
                DGMetas.Items.Refresh();
            }
            showMainDisplay();
        }
        #endregion Private EH

        #region GUI Event Handlers
        private void eOK_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "OK");
        }

        private void eCancel_Click(object sender, RoutedEventArgs e)
        {
            UnhandledEvent(DataModel, "Cancel");
        }

        private void eAddTag_Click(object sender, RoutedEventArgs e)
        {
            showKeyValCreaterWindow();
        }

        private void eRemoveTag_Click(object sender, RoutedEventArgs e)
        {
            if (DGMetas.SelectedIndex != -1)
            {
                DataModel.LstCurrentMetaTags.Remove(DataModel.LstCurrentMetaTags[DGMetas.SelectedIndex]);
                DGMetas.Items.Refresh();
            }
            showMainDisplay();
        }

        #endregion GUI EH
    }
}
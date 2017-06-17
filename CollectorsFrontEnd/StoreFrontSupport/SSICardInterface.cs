using CollectorsFrontEnd.InterfaceModels;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    public static partial class ServerInterface
    {
        public static class Card
        {
            public static string SZ_IMAGE_CACHE_PATH = SCI.GetImagesPath();

            private class ImageDownloadedEventArgs : EventArgs
            {
                public ImageDownloadedEventArgs(CardModel aDataModel, EventArgs ae)
                {
                    DataModel = aDataModel;
                    e = ae;
                }
                public CardModel DataModel;
                public EventArgs e;
            }

            // Compares long name
            public static bool AreCardsSame(CardModel aoCardOne, CardModel aoCardTwo)
            {
                if (aoCardOne.CardNameLong == null || aoCardTwo.CardNameLong == null)
                {
                    return false;
                }
                return SCI.IsSameIdentity(aoCardOne.CardNameLong, aoCardTwo.CardNameLong);
            }

            public static bool AreMetaTagsSame(List<Tuple<string, string>> alstTupOne, List<Tuple<string, string>> alstTupTwo)
            {
                return SCI.IsSameMetaTags(alstTupOne, alstTupTwo);
            }

            public static void AddMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                string aszVal,
                List<Tuple<string, string>> aLstMatchMeta)
            {
                SCI.AddMetaTag(aszCollectionName, aszLongCardName, aszKey, aszVal, aLstMatchMeta);
            }

            public static void SubmitMetaTagChangesToServer(string aszCollectionName, string aszLongName,
                List<Tuple<string, string>> alstNewMeta, List<Tuple<string, string>> alstMeta)
            {
                SCI.SetMetaTags(aszCollectionName, aszLongName, alstNewMeta, alstMeta);
            }

            public static void SubmitFeatureChangesToServer(string aszCollectionName, string aszLongName,
                List<Tuple<string, string>> alstNewMeta,
                List<Tuple<string, string>> alstNewAttrs,
                List<Tuple<string, string>> alstMeta)
            {
                SCI.SetFeatures(aszCollectionName, aszLongName, alstNewMeta, alstNewAttrs, alstMeta);
            }

            public static void RemoveMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                List<Tuple<string, string>> aLstMatchMeta)
            {
                SCI.RemoveMetaTag(aszCollectionName, aszLongCardName, aszKey, aLstMatchMeta);
            }

            // Passes the image to the callback.
            // There is notably a memory leak in wpf bitmap images. read here.
            //http://stackoverflow.com/questions/21877221/memory-not-getting-released-in-wpf-image
            // I used the ANTS profiler and it looks like I am not leaking memory. It just seems that
            // the process will hold onto memory after its no longer needed. So when viewing lots of images
            // we may not expect it to return to its previous usage.
            public static void DownloadAndCacheImage(EventHandler aeHandlerCallback, CardModel aoCardModel)
            {
                // Save in set

                string szMUID = aoCardModel.GetAttr("multiverseid");
                string szSet = aoCardModel.GetAttr("set");
                BitmapImage bi3;

                string szBasePath = SZ_IMAGE_CACHE_PATH + "\\" + szSet + "/";
                if (szMUID != "")
                {
                    szBasePath += szMUID + "/";
                }
                string szFilePath = szBasePath + aoCardModel.CardName + ".jpg";
                if (!File.Exists(szFilePath))
                {
                    bi3 = new BitmapImage();
                    bi3.DownloadCompleted += aeHandlerCallback;
                    bi3.DownloadFailed += eModelImage_DownloadFailed;
                    bi3.BeginInit();
                    bi3.DownloadCompleted +=
                            ((object sender, EventArgs e) =>
                            { eModelImage_DownloadCompleted(sender, new ImageDownloadedEventArgs(aoCardModel, e)); });
                    if (!string.IsNullOrEmpty(szMUID))
                    {
                        bi3.UriSource =
                            new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" +
                            szMUID + @"&type=card", UriKind.RelativeOrAbsolute);
                    }
                    else
                    {
                        bi3.UriSource =
                            new Uri(@"http://gatherer.wizards.com/Handlers/Image.ashx?name=" +
                            aoCardModel.CardName + "&type=card", UriKind.RelativeOrAbsolute);
                    }
                    bi3.CacheOption = BitmapCacheOption.OnLoad;
                    bi3.EndInit();
                }
                else
                {
                    string szFullPath = Path.GetFullPath(szFilePath);

                    bi3 = new BitmapImage();
                    FileStream stream = File.OpenRead(szFullPath);

                    bi3.BeginInit();
                    bi3.CacheOption = BitmapCacheOption.OnLoad;
                    bi3.StreamSource = stream;
                    bi3.EndInit();
                    bi3.Freeze();
                    aeHandlerCallback(bi3, null);
                }

            }

            private static void eModelImage_DownloadCompleted(object sender, ImageDownloadedEventArgs ie)
            {
                EventArgs e = ie.e;
                CardModel cardModel = ie.DataModel;
                string szMUID = cardModel.GetAttr("multiverseid");
                string szSet = cardModel.GetAttr("set");

                JpegBitmapEncoder encoder = new JpegBitmapEncoder();
                string szBasePath = SZ_IMAGE_CACHE_PATH + "\\" + szSet + "/";
                if (szMUID != "")
                {
                    szBasePath += szMUID + "/";
                }
                String photolocation = szBasePath + cardModel.CardName + ".jpg";
                encoder.Frames.Add(BitmapFrame.Create((BitmapImage)sender));

                if (!Directory.Exists(Path.GetDirectoryName(photolocation)))
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(photolocation));
                }

                using (var filestream = new FileStream(photolocation, FileMode.Create))
                    encoder.Save(filestream);
            }

            private static void eModelImage_DownloadFailed(object sender, EventArgs e)
            {

            }

            // Use sparingly. Eventually include attributes in metatags?
            public static List<string> GetCardAttributesRestrictions(string aszLongName, string aszKey)
            {
                return SCI.GetCardAttributeRestrictions(aszLongName, aszKey);
            }
        }
    }
}

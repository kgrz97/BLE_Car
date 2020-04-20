using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.Exceptions;
using Plugin.BLE.Abstractions.Extensions;
using Plugin.BLE.Abstractions.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.PlatformConfiguration;
using Syncfusion.SfGauge;

namespace App_test
{
    public partial class MainPage : ContentPage
    {
        static int P = 5, K = 5;
        static char T = '0';

        static int FirstMessage = 0;

        bool SearchBT = false;

        static int Signal_Strength = 0;

        public static int Battery_Status = 3;

        public Thread Battery_thread;

        public bool Can_connect = true;
       

        public class Start_Interface
        {
            public Image Scan;
            public Image Connect;
            public Image Close_StartView;
            public Image Credits;
            public Image Credits_view;
            
            public Image Disconnect;
            public Slider Steering_Slider;
            public Slider Throtle_Slider;
            public Syncfusion.SfGauge.XForms.SfCircularGauge Steering;
            public Syncfusion.SfGauge.XForms.SfCircularGauge Movement;
            public Image Signal_Image1;
            public Button Horn_Button;
            public Image Battery_Image;
            
            public Image Close_SteeringView;
            
            public void Start_HideAll()
            {
                Scan.IsVisible = false;
                Connect.IsVisible = false;
                Close_StartView.IsVisible = false;
                Credits.IsVisible = false;
                
                Disconnect.IsVisible = true;
                Steering_Slider.IsVisible = true;
                Throtle_Slider.IsVisible = true;
                Steering.IsVisible = true;
                Movement.IsVisible = true;
                Signal_Image1.IsVisible = true;
                Close_SteeringView.IsVisible = true;
                Horn_Button.IsVisible = true;
                Battery_Image.IsVisible = true;
            }

            public void Start_ShowAll()
            {
                Scan.IsVisible = true;

                Connect.Source = "CONNECT_disabled.jpg";
                Connect.IsVisible = true;
                Close_StartView.IsVisible = true;
                Credits.IsVisible = true;

                
                Disconnect.IsVisible = false;
                Steering_Slider.IsVisible = false;
                Throtle_Slider.IsVisible = false;
                Steering.IsVisible = false;
                Movement.IsVisible = false;
                Signal_Image1.IsVisible = false;
                Close_SteeringView.IsVisible = false;
                Horn_Button.IsVisible = false;
                Battery_Image.IsVisible = false;

                if(FirstMessage != 0)
                 chars.StopUpdatesAsync();
            }
        }
        
        IBluetoothLE ble;
        public static IAdapter adapter;
        public static Plugin.BLE.Abstractions.Contracts.IDevice device_connected;
        ObservableCollection<Plugin.BLE.Abstractions.Contracts.IDevice> deviceList;
        public static Plugin.BLE.Abstractions.Contracts.ICharacteristic chars;


        System.Timers.Timer CheckSignalTimer = new System.Timers.Timer(250);
        System.Timers.Timer timer = new System.Timers.Timer(80);

        Start_Interface SI = new Start_Interface();

        public MainPage()
        {
            InitializeComponent();

            ble = CrossBluetoothLE.Current;
            adapter = ble.Adapter;
            deviceList = new ObservableCollection<IDevice>();
            
            this.BackgroundImage = "background_1.jpg";
            
            P = K = 5;
            T = '0';

            
            SI.Scan = this.FindByName<Image>("Scan_Image");
            SI.Connect = this.FindByName<Image>("Connect_Image");
            SI.Close_StartView = this.FindByName<Image>("Close_Image");
            SI.Credits = this.FindByName<Image>("Credits_Image");
            SI.Credits_view = this.FindByName<Image>("Credits_view_Image");


            SI.Disconnect = this.FindByName<Image>("Disconnect_Image");
            SI.Steering_Slider = this.FindByName<Slider>("Steering_Slider");
            SI.Throtle_Slider = this.FindByName<Slider>("Throttle_Slider");
            SI.Steering = this.FindByName<Syncfusion.SfGauge.XForms.SfCircularGauge>("SteeringWheel");
            SI.Movement = this.FindByName<Syncfusion.SfGauge.XForms.SfCircularGauge>("Speedometer");
            SI.Signal_Image1 = this.FindByName<Image>("Signal_Image_1");
            SI.Horn_Button = this.FindByName<Button>("Horn_button");
            SI.Battery_Image = this.FindByName<Image>("Battery_Image");
            
            SI.Close_SteeringView = this.FindByName<Image>("Close_SV_Image");
            
            SI.Start_ShowAll();
            
            timer.Elapsed += OnTimedEvent;
            
            CheckSignalTimer.Elapsed += CheckSignal;

            FirstMessage = 1;
        }

        public static bool Battery_received = false;

        private static void OnTimedEvent(object source, EventArgs e)
        {
            SendData();
        }

        public void Battery_Func()
        {
            try
            {
                    chars.ValueUpdated += (o, args) =>
                    {
                        var bts = args.Characteristic.Value;
                        string recd = Encoding.UTF8.GetString(bts, 0, 3); // bts.Length

                        if (recd.Length == 3)
                        {
                            string rec = recd.Substring(0, 3);

                            if (rec == "BSA")
                                Battery_Status = 1;

                            if (rec == "BSB")
                                Battery_Status = 2;

                            if (rec == "BSC")
                                Battery_Status = 3;

                            Console.WriteLine("Received data: " + rec);
                        }
                    };
            }
            catch (Exception exc) { Console.WriteLine("Can't receive data!"); };
        }

        public async void CheckSignal(object source, EventArgs e)
        {
            Device.BeginInvokeOnMainThread(() =>
            {
                if (Signal_Strength != 0)
                    switch (Signal_Strength)
                    {
                        case 1: { SI.Signal_Image1.Source = "wifi_2.JPG"; } break;
                        case 2: { SI.Signal_Image1.Source = "wifi_3.JPG"; } break;
                        case 3: { SI.Signal_Image1.Source = "wifi_4.jpg"; } break;
                        default: { SI.Signal_Image1.Source = "wifi_4.jpg"; }; break;
                    }

                switch (Battery_Status)
                {
                    case 1: { SI.Battery_Image.Source = "battery_1.jpg"; } break;
                    case 2: { SI.Battery_Image.Source = "battery_2.jpg"; } break;
                    case 3: { SI.Battery_Image.Source = "battery_3.jpg"; } break;
                    default: { SI.Battery_Image.Source = "battery_3.jpg"; }; break;
                }
            });

           

            if (device_connected != null)
            {
                var rssiUpdate = await device_connected.UpdateRssiAsync();

                if (rssiUpdate)
                {
                    int rssi = device_connected.Rssi;
                  //  Console.WriteLine(rssi);
                  
                        if (rssi <= -10 && rssi >= -85)  // rssi >= -85
                        {
                            Signal_Strength = 3;
                        }


                        if (rssi <= -86 && rssi >= -95) // rssi >= 95
                        {
                            Signal_Strength = 2;
                        }


                        if (rssi <= -96)
                        {
                            Signal_Strength = 1;
                        }
                    
                }
            }
        }


        async void Scanning_Function(object sender, EventArgs args)
        {
            SI.Scan.Source = "SCAN_tapped.jpg";

            if (ble.IsOn)
            {
                adapter.ScanTimeout = 1000;
                adapter.ScanMode = ScanMode.LowPower;

                adapter.DeviceDiscovered += (s, a) =>
                {
                    try
                    {
                        if (a.Device.Name.Equals("BT05"))
                        {
                            deviceList.Add(a.Device);
                            SearchBT = true;
                        }

                    }
                    catch (Exception e) { SearchBT = false; }
                };

                await adapter.StartScanningForDevicesAsync();

                Thread.Sleep(1100);

                if (SearchBT)
                {
                    await DisplayAlert("Result", "Found BT Car", "OK");
                    SI.Connect.Source = "CONNECT.jpg";
                }
                else
                {
                    await DisplayAlert("Result", "Not found", "OK");
                    SI.Connect.Source = "CONNECT_disabled.jpg";
                }
            }
            else
            {
                await DisplayAlert("Bluetooth", "Please turn on your BT", "OK");
            }
            SI.Scan.Source = "SCAN.jpg";
        }

        public bool GS = false;

        public async void ConnectToDevice(IDevice device)
        {
            SI.Connect.Source = "CONNECT_tapped.jpg";
            try
            {
                await adapter.ConnectToDeviceAsync(device);
                
                GS = await GetServices();
            }
            catch (DeviceConnectionException ex)
            {
                await DisplayAlert("ConnectToDev", ex.Message, "OK");
                Can_connect = false;
            }

            if (Can_connect && GS)
            {
                await DisplayAlert("Connected", "Status:" + device.State, "OK");

                SI.Start_HideAll();
                this.BackgroundImage = "background_2.jpg";

                FirstMessage = 1;

                timer.Enabled = true;
                CheckSignalTimer.Enabled = true;

               

                Battery_thread = new Thread(Battery_Func);
                Battery_thread.Start();
            }

            Can_connect = true;

            SI.Connect.Source = "CONNECT.jpg";
        }
        
        public async Task<bool> GetServices()
        {
            try
            {
                var services = await device_connected.GetServiceAsync(Guid.Parse("0000FFE0-0000-1000-8000-00805F9B34FB"));
                var characteristics = await services.GetCharacteristicAsync(Guid.Parse("0000FFE1-0000-1000-8000-00805F9B34FB")); // wpisać adres mac bt
                chars = characteristics;
                var descriptors = await chars.GetDescriptorAsync(Guid.Parse("0000FFE1-0000-1000-8000-00805F9B34FB"));
                
                await chars.StartUpdatesAsync();
            } catch (Exception exc) { Console.WriteLine("Sth wrong, but ok"); }

            return true;
        }

        public async void DisconnectClicked(object sender, EventArgs args)
        {
            SI.Disconnect.Source = "DISCONNECT_tapped.jpg";

            try
            {
                bool ans = await DisplayAlert("Disconnect", "Are you sure?", "Yes", "No");

                if (ans)
                {
                    FirstMessage = 0;
                    
                    Battery_thread.Abort();

                    await adapter.DisconnectDeviceAsync(device_connected);

                    CheckSignalTimer.Enabled = false;

                    this.BackgroundImage = "background_1.jpg";
                    SI.Start_ShowAll();
                }
            } catch (Exception exc) { await DisplayAlert("Disconnect", exc.Message, "OK"); };

            SI.Disconnect.Source = "DISCONNECT.jpg";
        }

        public static void SendData()
        {
            Thread.Sleep(5);
        
            string data;
            
            if (FirstMessage == 1)
            {
                Thread.Sleep(1000);
                data = "HI!";
                FirstMessage = 2;
            }
            else
                data = P.ToString() + K.ToString() + T;
            

            byte[] bytes = new byte[20];
            bytes = Encoding.ASCII.GetBytes(data.ToCharArray());
            try
            {
               chars.WriteAsync(bytes);
            }
            catch (Exception e) { Thread.Sleep(5); }
        }
        

        public async void CloseApp_Click(object sender, EventArgs args)
        {
            SI.Close_StartView.Source = "CLOSE_tapped.jpg";
            SI.Close_SteeringView.Source = "CLOSE_tapped.jpg";
            bool ans = await DisplayAlert("Quit", "Are you sure?", "Yes", "No");

            if (ans)
            {
                Process.GetCurrentProcess().CloseMainWindow();
                Process.GetCurrentProcess().Close();
            }
            else
            {
                SI.Close_StartView.Source = "CLOSE.jpg";
                SI.Close_SteeringView.Source = "CLOSE.jpg";
            }
               
        }

        public void Value_slider(object sender, ValueChangedEventArgs e)
        {
            double val = e.NewValue;

            Marker.Value = val;

            P = (int)Math.Round(val);
        }

        public void Steering_value(object sender, ValueChangedEventArgs e)
        {
            double val = e.NewValue;

            MarkerSW.Value = val;

            K = (int)Math.Round(val);
        }

        public void Throttle_Slider_end_gesture(object sender, MR.Gestures.DownUpEventArgs e)
        {
            Throttle_Slider.Value = 5;
        }

        public void Steering_Slider_end_gesture(object sender, MR.Gestures.DownUpEventArgs e)
        {
            Steering_Slider.Value = 5;
        }

        public void Horn_Button_Pressed(object sender, EventArgs args)
        {
            T = '1';
            SI.Horn_Button.Image = "horn_tapped.png";
        }

        public void Horn_Button_Released(object sender, EventArgs args)
        {
            T = '0';
            SI.Horn_Button.Image = "horn_new.jpg";
        }

        public async void OnTapGesture(object sender, EventArgs args)
        {
            try
            {
                if (ble.IsOn)
                {
                    ConnectToDevice(deviceList[0]);
                    device_connected = deviceList[0];
                }
                else
                {
                    await DisplayAlert("Bluetooth", "Please turn on your BT", "OK");
                }
            } catch (Exception exc) { Console.WriteLine("Can't connect!!!"); };
        }

        public void Credits_view_click(object sender, EventArgs args)
        {
            SI.Credits_view.IsVisible = false;
        }

        public void Show_credits(object sender, EventArgs args)
        {
            SI.Credits.Source = "CREDITS_tapped.jpg";

            SI.Credits_view.IsVisible = true;

            SI.Credits.Source = "CREDITS.jpg";
        }
    }
}

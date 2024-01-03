using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
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
using System.Windows.Threading;

namespace ExConsole2
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly CancellationTokenSource _cancellation = new();

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        public MainWindow()
        {
            InitializeComponent();

            // for (int i = 0; i < 1000; ++i)
            // {
            //     addLogging("ここに表示されるテキストは選択してコピーできます 🌴");
            // }
            connectToPipe(_cancellation.Token);
        }

        public void addLogging(string text)
        {
            var textBox = new TextBox()
            {
                Text = text,
                IsReadOnly = true,
                TextWrapping = TextWrapping.Wrap,
                Margin = new Thickness(8, 2, 8, 2),
            };
            stackPanel.Children.Add(textBox);
        }

        private async Task connectToPipe(CancellationToken cancellation)
        {
            addLogging("Waiting...");
            using StreamReader reader = new StreamReader(Console.OpenStandardInput(), Encoding.UTF8);
            while (true)
            {
                var input = await reader.ReadLineAsync(cancellation);
                addLogging("🗨️ input");
                if (input == null)
                {
                    addLogging("😢 null");
                    await Task.Delay(1000, cancellation);
                    continue;
                }

                // UIスレッドで処理する必要がある場合
                Dispatcher.Invoke(() => { addLogging("📝 " + input); });
            }
        }

        private void onClosing(object? sender, CancelEventArgs e)
        {
            _cancellation.Cancel();
        }
    }
}
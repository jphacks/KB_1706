class CreateApiCustomers < ActiveRecord::Migration[5.1]
  def change
    create_table :api_customers do |t|
      t.string :coinadress
      t.string :onepass
      t.string :twopass

      t.timestamps
    end
  end
end
